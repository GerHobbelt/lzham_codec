// example2.c - Demonstrates single function call compression/decompression using the lzham_compress() and lzham_uncompress() functions.
// This example statically links against the lzhamlib, lzhamcomp, and lzhamdecomp libs.
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>

// Define LZHAM_DEFINE_ZLIB_API causes lzham.h to remap the standard zlib.h functions/macro definitions to lzham's.
// This is totally optional - you can also directly use the lzham_* functions and macros instead.
#define LZHAM_DEFINE_ZLIB_API
#include "lzham_static_lib.h"

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint;

// The string to compress.
static const char *s_pStr = "Good morning Dr. Chandra. This is Hal. I am ready for my first lesson." \
   "Good morning Dr. Chandra. This is Hal. I am ready for my first lesson." \
   "Good morning Dr. Chandra. This is Hal. I am ready for my first lesson." \
   "Good morning Dr. Chandra. This is Hal. I am ready for my first lesson." \
   "Good morning Dr. Chandra. This is Hal. I am ready for my first lesson." \
   "Good morning Dr. Chandra. This is Hal. I am ready for my first lesson." \
   "Good morning Dr. Chandra. This is Hal. I am ready for my first lesson." \
   "Hal... Dave and I believe that there's something about the mission that we weren't told." \
   "Something that the rest of the crew know and that you know. We'd like to know whether this is true.";

int main(int argc, const char **argv)
{
   uint step = 0;
   int cmp_status;
   uLong src_len = (uLong)strlen(s_pStr);
   uLong cmp_len = compressBound(src_len);
   uLong uncomp_len = src_len;
   uint8 *pCmp, *pUncomp;
   uint total_succeeded = 0;

   printf("LZHAM example2 (single function call compression/decompression)\nUsing library version: %s\n", ZLIB_VERSION);

   do
   {
      // Allocate buffers to hold compressed and uncompressed data.
      pCmp = (uint8 *)malloc((size_t)cmp_len);
      pUncomp = (uint8 *)malloc((size_t)src_len);
      if ((!pCmp) || (!pUncomp))
      {
         printf("Out of memory!\n");
         return EXIT_FAILURE;
      }

      // Compress the string.
      cmp_status = compress(pCmp, &cmp_len, (const unsigned char *)s_pStr, src_len);
      if (cmp_status != Z_OK)
      {
         printf("compress() failed!\n");
         free(pCmp);
         free(pUncomp);
         return EXIT_FAILURE;
      }

      printf("Compressed from %u to %u bytes\n", src_len, cmp_len);

      if (step)
      {
         // Purposely corrupt the compressed data if fuzzy testing (this is a very crude fuzzy test).
         uint n = 1 + (rand() % 3);
         while (n--)
         {
            uint i = rand() % cmp_len;
            pCmp[i] ^= (rand() & 0xFF);
         }
      }

      // Decompress.
      cmp_status = uncompress(pUncomp, &uncomp_len, pCmp, cmp_len);
      total_succeeded += (cmp_status == Z_OK);

      if (step)
      {
         printf("Simple fuzzy test: step %u total_succeeded: %u\n", step, total_succeeded);
      }
      else
      {
         if (cmp_status != Z_OK)
         {
            printf("uncompress failed!\n");
            free(pCmp);
            free(pUncomp);
            return EXIT_FAILURE;
         }

         printf("Decompressed from %u to %u bytes\n", cmp_len, uncomp_len);

         // Ensure uncompress() returned the expected data.
         if ((uncomp_len != src_len) || (memcmp(pUncomp, s_pStr, (size_t)src_len)))
         {
            printf("Decompression failed!\n");
            free(pCmp);
            free(pUncomp);
            return EXIT_FAILURE;
         }
      }

      free(pCmp);
      free(pUncomp);

      step++;

      // Keep on fuzzy testing if there's a non-empty command line.
   } while (argc >= 2);  

   printf("Success.\n");
   return EXIT_SUCCESS;
}
