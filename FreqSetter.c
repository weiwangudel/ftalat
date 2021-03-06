/*
 * ftalat - Frequency Transition Latency Estimator
 * Copyright (C) 2013 Universite de Versailles
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "FreqSetter.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include <unistd.h>

#include "rdtsc.h"

#include "FreqGetter.h"

#include "utils.h"

FILE** pSetFiles = NULL;
int* pSetMSRs = NULL;

char openFreqSetterFiles()
{
   unsigned int nbCore = getCoreNumber();
      
   pSetFiles = malloc(sizeof(FILE*) * nbCore);
   
   if (pSetFiles == NULL)
   {
      fprintf(stdout,"Fail to allocate memory for files\n");
      return -1;
   }
   
   unsigned int i = 0;
   for ( i = 0 ; i < nbCore ; i++ )
   {
      pSetFiles[i] = openCPUFreqFile(i,"scaling_setspeed","w");
      if ( pSetFiles[i] == NULL )
      {
         return -1;
      }
   }
   
   return 0;
}

char openDCMSetterMSRs()
{
   unsigned int nbCore = getCoreNumber();

   pSetMSRs = malloc (sizeof(int) * nbCore);
   
   if (pSetMSRs == NULL)
   {
      fprintf(stdout,"Fail to allocate memory for file descriptors\n");
      return -1;
   }
     
   unsigned int i = 0;
   for ( i = 0 ; i < nbCore ; i++ )
   {
      pSetMSRs[i] = openCPUMSR(i);
      printf("pSetMSR[%d]=%d\n", i, pSetMSRs[i]);
      if ( pSetMSRs[i] < 0 )
      {
         return -1;
      }
   }
   
   return 0;

}

inline void setFreq(unsigned int coreID, unsigned int targetFreq)
{
   assert(coreID < getCoreNumber());
   
   fprintf(pSetFiles[coreID],"%d",targetFreq);
   fflush(pSetFiles[coreID]);
}


inline void setDCM(unsigned int coreID, uint64_t targetDCM)
{
   assert(coreID < getCoreNumber());
   
   printf("Begin Setting Core %d to %x", coreID, targetDCM);
   pwrite(pSetMSRs[coreID], &targetDCM, sizeof targetDCM, 0x19A);
   printf("Finish Setting Core %d to %x", coreID, targetDCM);
}

inline void setAllFreq(unsigned int targetFreq)
{
   int nbCore = getCoreNumber();
   int i;
   
   for (i = 0; i < nbCore ; i++ )
   {
      fprintf(pSetFiles[i],"%d",targetFreq);
      fflush(pSetFiles[i]);
   }
}

inline void setAllDCM(unsigned int targetDCM)
{
   int nbCore = getCoreNumber();
   int i;
   
   for (i = 0; i < nbCore ; i++ )
   {
      pwrite(pSetMSRs[i], &targetDCM, sizeof targetDCM, 0x19a);
   }
}

inline void setMinFreqForAll()
{
   setAllFreq(getMinAvailableFreq(0));
   waitCurFreq(0,getMinAvailableFreq(0));
}


void closeFreqSetterFiles(void) 
{
   int nbCore = getCoreNumber();
   int i = 0;
   
   if ( pSetFiles )
   {
      for ( i = 0 ; i < nbCore ; i++ )
      {
         if ( pSetFiles[i] )
         {
            fclose(pSetFiles[i]);
         }
      }
      
      free(pSetFiles);
   }
}

void closeDCMSetterMSRs(void) 
{
   int nbCore = getCoreNumber();
   int i = 0;
   
   if ( pSetMSRs )
   {
      for ( i = 0 ; i < nbCore ; i++ )
      {
         if ( pSetMSRs[i] >= 0 )
         {
            close(pSetMSRs[i]);
         }
      }
      
      free(pSetMSRs);
   }
}

char setCPUGovernor(const char* newPolicy)
{
   int nbCore = getCoreNumber();
   int i = 0;
   
   assert(newPolicy);
   
   for ( i = 0 ; i < nbCore ; i++ )
   {
      FILE* pGovernorFile = openCPUFreqFile(i,"scaling_governor","w");
      if ( pGovernorFile != NULL )
      {
         fprintf(pGovernorFile,"%s",newPolicy);
      
         fclose(pGovernorFile);
      }
      else
      {
         return -1;
      }
   }
   
   return 0;
}

