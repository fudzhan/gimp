/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "config.h"

#include <glib.h>

#include <stdio.h> /* SEEK_SET */
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef USE_PTHREADS
#include <pthread.h>
#endif

#ifdef G_OS_WIN32
#include <io.h>
#endif

#ifndef _O_BINARY
#define _O_BINARY 0
#endif
#ifndef _O_TEMPORARY
#define _O_TEMPORARY 0
#endif

#include "apptypes.h"

#include "tile.h"
#include "tile_pvt.h"
#include "tile_swap.h"


#define MAX_OPEN_SWAP_FILES  16

typedef struct _SwapFile      SwapFile;
typedef struct _DefSwapFile   DefSwapFile;
typedef struct _Gap           Gap;
typedef struct _AsyncSwapArgs AsyncSwapArgs;

struct _SwapFile
{
  gchar    *filename;
  gint      swap_num;
  SwapFunc  swap_func;
  gpointer  user_data;
  gint      fd;
};

struct _DefSwapFile
{
  GList *gaps;
  glong  swap_file_end;
  off_t  cur_position;
};

struct _Gap
{
  glong start;
  glong end;
};

struct _AsyncSwapArgs
{
  DefSwapFile *def_swap_file;
  gint         fd;
  Tile        *tile;
};


static void    tile_swap_init             (void);
static guint   tile_swap_hash             (gint        *key);
static gint    tile_swap_compare          (gint        *a,
					   gint        *b);
static void    tile_swap_command          (Tile        *tile,
					   gint         command);
static void    tile_swap_open             (SwapFile    *swap_file);

static gint    tile_swap_default          (gint         fd,
					   Tile        *tile,
					   gint         cmd,
					   gpointer     user_data);
static void    tile_swap_default_in       (DefSwapFile *def_swap_file,
					   gint         fd,
					   Tile        *tile);
static void    tile_swap_default_in_async (DefSwapFile *def_swap_file,
					   gint         fd,
					   Tile        *tile);
static void    tile_swap_default_out      (DefSwapFile *def_swap_file,
					   gint         fd,
					   Tile        *tile);
static void    tile_swap_default_delete   (DefSwapFile *def_swap_file,
					   gint         fd,
					   Tile        *tile);
static glong   tile_swap_find_offset      (DefSwapFile *def_swap_file,
					   gint         fd,
					   gint         bytes);
static void    tile_swap_resize           (DefSwapFile *def_swap_file,
					   gint         fd,
					   glong        new_size);
static Gap   * tile_swap_gap_new          (glong        start,
					   glong        end);
static void    tile_swap_gap_destroy      (Gap         *gap);
#ifdef USE_PTHREADS
static gpointer tile_swap_in_thread       (gpointer);
#endif


static gboolean        initialize       = TRUE;
static GHashTable    * swap_files       = NULL;
static GList         * open_swap_files  = NULL;
static gint            nopen_swap_files = 0;
static gint            next_swap_num    = 1;
static glong           swap_file_grow   = 16 * TILE_WIDTH * TILE_HEIGHT * 4;
#ifdef USE_PTHREADS
static pthread_mutex_t swapfile_mutex         = PTHREAD_MUTEX_INITIALIZER;

/* async_swapin_mutex protects only the list, not the tiles therein */
static pthread_t       swapin_thread;
static pthread_mutex_t async_swapin_mutex     = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  async_swapin_signal    = PTHREAD_COND_INITIALIZER;
static GSList        * async_swapin_tiles     = NULL;
static GSList        * async_swapin_tiles_end = NULL;
#endif

static gboolean seek_err_msg = TRUE, read_err_msg = TRUE, write_err_msg = TRUE;


static void
tile_swap_print_gaps (DefSwapFile *def_swap_file)
{
  GList *gaps;
  Gap   *gap;

  gaps = def_swap_file->gaps;
  while (gaps)
    {
      gap = gaps->data;
      gaps = gaps->next;

      g_print ("  %6ld - %6ld\n", gap->start, gap->end);
    }
}

static void
tile_swap_exit1 (gpointer key,
		 gpointer value,
		 gpointer data)
{
  extern gint  tile_ref_count;
  SwapFile    *swap_file;
  DefSwapFile *def_swap_file;

  if (tile_ref_count != 0)
    g_warning ("tile ref count balance: %d\n", tile_ref_count);

  swap_file = value;
  if (swap_file->swap_func == tile_swap_default)
    {
      def_swap_file = swap_file->user_data;
      if (def_swap_file->swap_file_end != 0)
	{
	  g_warning ("swap file not empty: \"%s\"\n", swap_file->filename);
	  tile_swap_print_gaps (def_swap_file);
	}

#if defined (__EMX__) || defined (G_OS_WIN32)
      /* should close before unlink */
      if (swap_file->fd > 0)
	{
	  close (swap_file->fd);
	  swap_file->fd = -1;
	}
#endif
      unlink (swap_file->filename);
    }
}

void
tile_swap_exit (void)
{
#ifdef HINTS_SANITY
  extern int tile_exist_peak;

  g_printerr ("Tile exist peak was %d Tile structs (%d bytes)",
	      tile_exist_peak, tile_exist_peak * sizeof(Tile));
#endif

  if (swap_files)
    g_hash_table_foreach (swap_files, tile_swap_exit1, NULL);
}

gint
tile_swap_add (gchar    *filename,
	       SwapFunc  swap_func,
	       gpointer  user_data)
{
  SwapFile    *swap_file;
  DefSwapFile *def_swap_file;

#ifdef USE_PTHREADS
  pthread_mutex_lock(&swapfile_mutex);
#endif

  if (initialize)
    tile_swap_init ();

  swap_file = g_new (SwapFile, 1);
  swap_file->filename = g_strdup (filename);
  swap_file->swap_num = next_swap_num++;

  if (!swap_func)
    {
      swap_func = tile_swap_default;

      def_swap_file = g_new (DefSwapFile, 1);
      def_swap_file->gaps = NULL;
      def_swap_file->swap_file_end = 0;
      def_swap_file->cur_position = 0;

      user_data = def_swap_file;
    }

  swap_file->swap_func = swap_func;
  swap_file->user_data = user_data;
  swap_file->fd = -1;

  g_hash_table_insert (swap_files, &swap_file->swap_num, swap_file);

#ifdef USE_PTHREADS
  pthread_mutex_unlock(&swapfile_mutex);
#endif
  return swap_file->swap_num;
}

void
tile_swap_remove (gint swap_num)
{
  SwapFile *swap_file;

#ifdef USE_PTHREADS
  pthread_mutex_lock(&swapfile_mutex);
#endif

  if (initialize)
    tile_swap_init ();

  swap_file = g_hash_table_lookup (swap_files, &swap_num);
  if (!swap_file)
    goto out;

  g_hash_table_remove (swap_files, &swap_num);

  if (swap_file->fd != -1)
    close (swap_file->fd);

  g_free (swap_file);
out:
#ifdef USE_PTHREADS
  pthread_mutex_unlock(&swapfile_mutex);
#endif
  return;
}

void
tile_swap_in_async (Tile *tile)
{
  if (tile->swap_offset == -1)
    return;

  tile_swap_command (tile, SWAP_IN_ASYNC);
}

void
tile_swap_in (Tile *tile)
{
  if (tile->swap_offset == -1)
    {
      tile_alloc (tile);
      return;
    }

  tile_swap_command (tile, SWAP_IN);
}

void
tile_swap_out (Tile *tile)
{
  tile_swap_command (tile, SWAP_OUT);
}

void
tile_swap_delete (Tile *tile)
{
  tile_swap_command (tile, SWAP_DELETE);
}

void
tile_swap_compress (gint swap_num)
{
  tile_swap_command (NULL, SWAP_COMPRESS);
}

static void
tile_swap_init (void)
{

  if (initialize)
    {
      initialize = FALSE;

      swap_files = g_hash_table_new ((GHashFunc) tile_swap_hash,
				     (GCompareFunc) tile_swap_compare);

#ifdef NOTDEF /* USE_PTHREADS */
      pthread_create (&swapin_thread, NULL, &tile_swap_in_thread, NULL);
#endif
    }
}

static guint
tile_swap_hash (gint *key)
{
  return (guint) *key;
}

static gint
tile_swap_compare (gint *a,
		   gint *b)
{
  return (*a == *b);
}

static void
tile_swap_command (Tile *tile,
		   gint  command)
{
  SwapFile *swap_file;
#ifdef USE_PTHREADS
  pthread_mutex_lock(&swapfile_mutex);
#endif

  if (initialize)
    tile_swap_init ();

  do {
    swap_file = g_hash_table_lookup (swap_files, &tile->swap_num);
    if (!swap_file)
      {
	g_warning ("could not find swap file for tile");
	goto out;
      }

    if (swap_file->fd == -1)
      {
	tile_swap_open (swap_file);

	if (swap_file->fd == -1)
	  goto out;
      }
  } 
  while ((* swap_file->swap_func) (swap_file->fd, 
				   tile, command, swap_file->user_data));

out:
#ifdef USE_PTHREADS
  pthread_mutex_unlock(&swapfile_mutex);
#endif

  return;
}

static void
tile_swap_open (SwapFile *swap_file)
{
  SwapFile *tmp;

  if (swap_file->fd != -1)
    return;

  if (nopen_swap_files == MAX_OPEN_SWAP_FILES)
    {
      tmp = open_swap_files->data;
      close (tmp->fd);
      tmp->fd = -1;

      open_swap_files = g_list_remove (open_swap_files, open_swap_files->data);
      nopen_swap_files -= 1;
    }

  swap_file->fd = open (swap_file->filename,
      			O_CREAT | O_RDWR | _O_BINARY | _O_TEMPORARY,
			S_IREAD | S_IWRITE);

  if (swap_file->fd == -1)
    {
      g_message ("unable to open swap file...BAD THINGS WILL HAPPEN SOON");
      return;
    }

  open_swap_files = g_list_append (open_swap_files, swap_file);
  nopen_swap_files += 1;
}

/* The actual swap file code. The swap file consists of tiles
 *  which have been moved out to disk in order to conserve memory.
 *  The swap file format is free form. Any tile in memory may
 *  end up anywhere on disk.
 * An actual tile in the swap file consists only of the tile data.
 *  The offset of the tile on disk is stored in the tile data structure
 *  in memory.
 */

static int
tile_swap_default (gint      fd,
		   Tile     *tile,
		   gint      cmd,
		   gpointer  user_data)
{
  DefSwapFile *def_swap_file;

  def_swap_file = (DefSwapFile*) user_data;

  switch (cmd)
    {
    case SWAP_IN:
      tile_swap_default_in (def_swap_file, fd, tile);
      break;
    case SWAP_IN_ASYNC:
      tile_swap_default_in_async (def_swap_file, fd, tile);
      break;
    case SWAP_OUT:
      tile_swap_default_out (def_swap_file, fd, tile);
      break;
    case SWAP_DELETE:
      tile_swap_default_delete (def_swap_file, fd, tile);
      break;
    case SWAP_COMPRESS:
      g_warning ("tile_swap_default: SWAP_COMPRESS: UNFINISHED");
      break;
    }

  return FALSE;
}

static void
tile_swap_default_in_async (DefSwapFile *def_swap_file,
		            gint         fd,
		            Tile        *tile)
{
#ifdef NOTDEF /* USE_PTHREADS */
  AsyncSwapArgs *args;

  args = g_new(AsyncSwapArgs, 1);
  args->def_swap_file = def_swap_file;
  args->fd = fd;
  args->tile = tile;

  /* add this tile to the list of tiles for the async swapin task */
  pthread_mutex_lock (&async_swapin_mutex);
  g_slist_append (async_swapin_tiles_end, args);

  if (!async_swapin_tiles)
    async_swapin_tiles = async_swapin_tiles_end;
  
  pthread_cond_signal (&async_swapin_signal);
  pthread_mutex_unlock (&async_swapin_mutex);

#else
  /* ignore; it's only a hint anyway */
  /* this could be changed to call out to another program that
   * tries to make the OS read the data in from disk.
   */
#endif

  return;
}

/* NOTE: if you change this function, check to see if your changes
 * apply to tile_swap_in_attempt() near the end of the file.  The
 * difference is that this version makes guarantees about what it
 * provides, but tile_swap_in_attempt() just tries and gives up if
 * anything goes wrong.
 *
 * I'm not sure that it is worthwhile to try to pull out common
 * bits; I think the two functions are (at least for now) different
 * enough to keep as two functions.
 *
 * N.B. the mutex on the tile must already have been locked on entry
 * to this function.  DO NOT LOCK IT HERE.
 */
static void
tile_swap_default_in (DefSwapFile *def_swap_file,
		      gint         fd,
		      Tile        *tile)
{
  gint  bytes;
  gint  err;
  gint  nleft;
  off_t offset;

  err = -1;

  if (tile->data)
    {
      return;
    }

  if (def_swap_file->cur_position != tile->swap_offset)
    {
      def_swap_file->cur_position = tile->swap_offset;

      offset = lseek (fd, tile->swap_offset, SEEK_SET);
      if (offset == -1)
	{
	  if (seek_err_msg)
	    g_message ("unable to seek to tile location on disk: %d", err);
	  seek_err_msg = FALSE;
	  return;
	}
    }

  bytes = tile_size (tile);
  tile_alloc (tile);

  nleft = bytes;
  while (nleft > 0)
    {
      do 
	{
	  err = read (fd, tile->data + bytes - nleft, nleft);
	} 
      while ((err == -1) && ((errno == EAGAIN) || (errno == EINTR)));

      if (err <= 0)
	{
	  if (read_err_msg)
	    g_message ("unable to read tile data from disk: %s (%d/%d bytes read)", 
		       g_strerror (errno), err, nleft);
	  read_err_msg = FALSE;
	  return;
	}

      nleft -= err;
    }

  def_swap_file->cur_position += bytes;

  /*  Do not delete the swap from the file  */
  /*  tile_swap_default_delete (def_swap_file, fd, tile);  */

  read_err_msg = seek_err_msg = TRUE;
}

static void
tile_swap_default_out (DefSwapFile *def_swap_file,
		       int          fd,
		       Tile        *tile)
{
  gint  bytes;
  gint  rbytes;
  gint  err;
  gint  nleft;
  off_t offset;
  off_t newpos;

  bytes = TILE_WIDTH * TILE_HEIGHT * tile->bpp;
  rbytes = tile_size (tile);

  /*  If there is already a valid swap_offset, use it  */
  if (tile->swap_offset == -1)
    newpos = tile_swap_find_offset (def_swap_file, fd, bytes);
  else 
    newpos = tile->swap_offset;

  if (def_swap_file->cur_position != newpos)
    {
      offset = lseek (fd, newpos, SEEK_SET);
      if (offset == -1)
	{
	  if (seek_err_msg)
	    g_message ("unable to seek to tile location on disk: %s", 
		       g_strerror (errno));
	  seek_err_msg = FALSE;
	  return;
	}
      def_swap_file->cur_position = newpos;
    }

  nleft = rbytes;
  while (nleft > 0)
    {
      err = write (fd, tile->data + rbytes - nleft, nleft);
      if (err <= 0)
	{
	  if (write_err_msg)
	    g_message ("unable to write tile data to disk: %s (%d/%d bytes written)", 
		       g_strerror (errno), err, nleft);
	  write_err_msg = FALSE;
	  return;
	}

      nleft -= err;
    }

  def_swap_file->cur_position += rbytes;

  /* Do NOT free tile->data because we may be pre-swapping.
   * tile->data is freed in tile_cache_zorch_next
   */
  tile->dirty = FALSE;
  tile->swap_offset = newpos;

  write_err_msg = seek_err_msg = TRUE;
}

static void
tile_swap_default_delete (DefSwapFile *def_swap_file,
			  gint         fd,
			  Tile        *tile)
{
  GList *tmp;
  GList *tmp2;
  Gap   *gap;
  Gap   *gap2;
  glong  start;
  glong  end;

  if (tile->swap_offset == -1)
    return;

  start = tile->swap_offset;
  end = start + TILE_WIDTH * TILE_HEIGHT * tile->bpp;
  tile->swap_offset = -1;

  tmp = def_swap_file->gaps;
  while (tmp)
    {
      gap = tmp->data;

      if (end == gap->start)
	{
	  gap->start = start;

	  if (tmp->prev)
	    {
	      gap2 = tmp->prev->data;
	      if (gap->start == gap2->end)
		{
		  gap2->end = gap->end;
		  tile_swap_gap_destroy (gap);
		  def_swap_file->gaps = 
		    g_list_remove_link (def_swap_file->gaps, tmp);
		  g_list_free (tmp);
		}
	    }
	  break;
	}
      else if (start == gap->end)
	{
	  gap->end = end;

	  if (tmp->next)
	    {
	      gap2 = tmp->next->data;
	      if (gap->end == gap2->start)
		{
		  gap2->start = gap->start;
		  tile_swap_gap_destroy (gap);
		  def_swap_file->gaps = 
		    g_list_remove_link (def_swap_file->gaps, tmp);
		  g_list_free (tmp);
		}
	    }
	  break;
	}
      else if (end < gap->start)
	{
	  gap = tile_swap_gap_new (start, end);

	  tmp2 = g_list_alloc ();
	  tmp2->data = gap;
	  tmp2->next = tmp;
	  tmp2->prev = tmp->prev;
	  if (tmp->prev)
	    tmp->prev->next = tmp2;
	  tmp->prev = tmp2;

	  if (tmp == def_swap_file->gaps)
	    def_swap_file->gaps = tmp2;
	  break;
	}
      else if (!tmp->next)
	{
	  gap = tile_swap_gap_new (start, end);
	  tmp->next = g_list_alloc ();
	  tmp->next->data = gap;
	  tmp->next->prev = tmp;
	  break;
	}

      tmp = tmp->next;
    }

  if (!def_swap_file->gaps)
    {
      gap = tile_swap_gap_new (start, end);
      def_swap_file->gaps = g_list_append (def_swap_file->gaps, gap);
    }

  tmp = g_list_last (def_swap_file->gaps);
  gap = tmp->data;

  if (gap->end == def_swap_file->swap_file_end)
    {
      tile_swap_resize (def_swap_file, fd, gap->start);
      tile_swap_gap_destroy (gap);
      def_swap_file->gaps = g_list_remove_link (def_swap_file->gaps, tmp);
      g_list_free (tmp);
    }
}

static void
tile_swap_resize (DefSwapFile *def_swap_file,
		  gint         fd,
		  glong        new_size)
{
  if (def_swap_file->swap_file_end > new_size)
    {
      ftruncate (fd, new_size);
    }

  def_swap_file->swap_file_end = new_size;
}

static long
tile_swap_find_offset (DefSwapFile *def_swap_file,
		       gint         fd,
		       gint         bytes)
{
  GList *tmp;
  Gap   *gap;
  glong  offset;

  tmp = def_swap_file->gaps;
  while (tmp)
    {
      gap = tmp->data;

      if ((gap->end - gap->start) >= bytes)
	{
	  offset = gap->start;
	  gap->start += bytes;

	  if (gap->start == gap->end)
	    {
	      tile_swap_gap_destroy (gap);
	      def_swap_file->gaps = 
		g_list_remove_link (def_swap_file->gaps, tmp);
	      g_list_free (tmp);
	    }

	  return offset;
	}

      tmp = tmp->next;
    }

  offset = def_swap_file->swap_file_end;

  tile_swap_resize (def_swap_file, fd, 
		    def_swap_file->swap_file_end + swap_file_grow);

  if ((offset + bytes) < (def_swap_file->swap_file_end))
    {
      gap = tile_swap_gap_new (offset + bytes, def_swap_file->swap_file_end);
      def_swap_file->gaps = g_list_append (def_swap_file->gaps, gap);
    }

  return offset;
}

static Gap *
tile_swap_gap_new (glong start,
		   glong end)
{
  Gap *gap;

  gap = g_new (Gap, 1);
  gap->start = start;
  gap->end = end;

  return gap;
}

static void
tile_swap_gap_destroy (Gap *gap)
{
  g_free (gap);
}


#ifdef NOTDEF /* USE_PTHREADS */
/* go through the list of tiles that are likely to be used soon and
 * try to swap them in.  If any tile is not in a state to be swapped
 * in, ignore it, and the error will get dealt with when the tile
 * is really needed -- assuming that the error still happens.
 *
 * Potential future enhancement: for non-threaded systems, we could
 * fork() a process which merely attempts to bring tiles into the
 * OS's buffer/page cache, where they will be read into the gimp
 * more quickly.  This would be pretty trivial, actually.
 */

static void
tile_swap_in_attempt (DefSwapFile *def_swap_file,
		      gint         fd,
		      Tile        *tile)
{
  gint  bytes;
  gint  err;
  gint  nleft;
  off_t offset;

  err = -1;

  TILE_MUTEX_LOCK (tile);
  if (tile->data)
    goto out;

  if (!tile->swap_num || !tile->swap_offset)
    goto out;

  if (def_swap_file->cur_position != tile->swap_offset)
    {
      def_swap_file->cur_position = tile->swap_offset;

      offset = lseek (fd, tile->swap_offset, SEEK_SET);
      if (offset == -1)
	return;
    }

  bytes = tile_size (tile);
  tile_alloc (tile);

  nleft = bytes;
  while (nleft > 0)
    {
      do 
	{
	  err = read (fd, tile->data + bytes - nleft, nleft);
	} 
      while ((err == -1) && ((errno == EAGAIN) || (errno == EINTR)));

      if (err <= 0)
        {
	  g_free (tile->data);
	  return;
        }

      nleft -= err;
    }

  def_swap_file->cur_position += bytes;

out:
  TILE_MUTEX_UNLOCK (tile);
}

static gpointer
tile_swap_in_thread (gpointer data)
{
  AsyncSwapArgs *args;
  GSList        *free_item;

  while (TRUE)
    {
      pthread_mutex_lock (&async_swapin_mutex);

      if (!async_swapin_tiles)
        {
          pthread_cond_wait (&async_swapin_signal, &async_swapin_mutex);
        }
      args = async_swapin_tiles->data;

      free_item = async_swapin_tiles;
      async_swapin_tiles = async_swapin_tiles->next;
      g_slist_free_1(free_item);

      if (!async_swapin_tiles)
        async_swapin_tiles_end = NULL;

      pthread_mutex_unlock (&async_swapin_mutex);

      tile_swap_in_attempt(args->def_swap_file, args->fd, args->tile);

      g_free(args);
    }
}
#endif
