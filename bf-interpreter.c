/***********************************************************************

   bf-interpreter.c -- A fully functional Brainfuck interpreter
   Copyright (C) 2014 DIban Hakistron.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
  
int
main (int argc, char *argv[])
{
  /* Prolog */
  char array[30000];
  char *ptr = array;
 
  int inst;
  int numo_braces = 0;
  fpos_t positions[1024];
  fpos_t *pos = positions;
  FILE *file = NULL;

  if (argc > 2)
    {
      fprintf (stderr, "Usage: %s [FILE]\n", argv[0]);
      exit (EXIT_FAILURE);
    }
  
  if (--argc > 0)
    {
      file = fopen (argv[1], "r");
      if (file == NULL)
	{
	  perror ("Error while opening file");
	  exit (EXIT_FAILURE);
	}
    }
  else
    {
      file = tmpfile ();
      if (file == NULL)
	{
	  perror ("Error while creating tmpfile");
	  exit (EXIT_FAILURE);
	}

      printf ("%% ");
      while ((inst = getchar ()) != EOF)
	{
	  if (inst == '\n')
	    printf ("%% ");
	  
	  putc (inst, file);
	}
      printf ("\n--Start--\n");

      fseek (file, 0, SEEK_SET);
    }
  
  memset (array, 0, 30000);
  memset (positions, 0, 1024);


  /* Main Part */
  while ((inst = getc (file)) != EOF)
    {
      switch (inst)
	{
	case '[':
	  ++numo_braces;
	  break;
	case ']':
	  --numo_braces;
	  break;
	}
      
      if (numo_braces < 0)
	{
	  fprintf (stderr, "Error: unmatched ']'\n");
	  fclose (file);
      	  exit (EXIT_FAILURE);
	}
    }

  if (numo_braces > 0)
    {
      fprintf (stderr, "Error: unmatched '['\n");
      fclose (file);
      exit (EXIT_FAILURE);
    }
  
  
  fseek (file, 0, SEEK_SET);
  
  while ((inst = getc (file)) != EOF)
    {
      switch (inst)
	{
	case '>':
	  ++ptr;
	  break;
	case '<':
	  --ptr;
	  break;
	case '+':
	  ++*ptr;
	  break;
	case '-':
	  --*ptr;
	  break;
	case '.':
	  putchar (*ptr);
	  break;
	case ',':
	  *ptr = getchar ();
	  break;
	case '[':
	  if (*ptr != 0)
	    fgetpos (file, (++pos));
	  else
	    {
	      numo_braces = 1;
	      while (numo_braces > 0)
		{
		  switch ((char)getc (file))
		    {
		    case '[':
		      ++numo_braces;
		      break;
		    case ']':
		      --numo_braces;
		      break;
		    }
		}
	    }
	  break;
	case ']':
	  if (*ptr != 0)
	    fsetpos (file, pos);
	  else
	    --pos;
	    break;
	case '#':
	case ';':
	  while (getc (file) != '\n');
	}
    }

  fclose (file);

  return EXIT_SUCCESS;
}
