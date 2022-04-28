#include <stdio.h>
#include "bmsDataTransmitter.h"

/* global variables */
batteryReading_st bmsTemperatureSocData;

/*
 **********************************************************************************************************************
 * Service name         : TransmitBmsData
 * Syntax               : void TransmitBmsData(void)
 * param[in]            : None
 * param[out]           : None
 * return               : None
 * Description          : This function prints the battery temperature and soc data read from file to the console
 **********************************************************************************************************************
 */
void TransmitBmsData(void)
{
  int loopCntr = 0;
  while(loopCntr < bmsTemperatureSocData.numOfData)
  {
    printf("Temperature = %f, StateOfCharge = %f\n",bmsTemperatureSocData.batteryTempearature[loopCntr],bmsTemperatureSocData.batterySoc[loopCntr]);
    loopCntr++;
  }
}

/*
 **********************************************************************************************************************
 * Service name         : readBmsData
 * Syntax               : retBmsStatus_en readBmsData(int runTimeIpNo)
 * param[in]            : runTimeIpNo
 * return               : OK_STATUS, ERROR_STATUS
 * Description          : Function which reads temperature, state of charge from a recorded 
                          file and reports success or failure
 **********************************************************************************************************************
 */
retBmsStatus_en readBmsData(int runTimeIpNo)
{
  retBmsStatus_en bmsStatusRet = ERROR_STATUS;
  FILE * fileToOpen= fopen("./Transmitter/BmsDataFile.txt","r");
  /* check if file is valid or not */
  if (fileToOpen)
  {
    bmsStatusRet = performBmsDataRead(fileToOpen, runTimeIpNo);
  }
  else
  {
	printf("Could not open file and reporting error status\n");
  }	
  fclose(fileToOpen);
  return bmsStatusRet;
}

/*
 **********************************************************************************************************************
 * Service name         : performBmsDataRead
 * Syntax               : retBmsStatus_en performBmsDataRead(FILE * fileToBeRead,int runTimeIpNum)
 * param[inout]         : fileToBeRead
 * param[in]            : runTimeIpNum
 * return               : OK_STATUS, ERROR_STATUS
 * Description          : Program to read battery health that includes temperature, state of charge from a recorded 
                          file
 **********************************************************************************************************************
 */
retBmsStatus_en performBmsDataRead(FILE * fileToBeRead,int runTimeIpNum)
{
  retBmsStatus_en bmsStatusRet = ERROR_STATUS;
  float dataTemperature = 0.0;
  float dataSoc = 0.0;
  int cntrLoop = 0;
  int tempRunTimeIp = runTimeIpNum;
  bmsTemperatureSocData.numOfData = 0; 
  int runTimeIp = 0;
  for(;fscanf(fileToBeRead, "%f %f\n", &dataTemperature,&dataSoc)!=EOF;cntrLoop++)
  {
    bmsTemperatureSocData.batteryTempearature[cntrLoop] = dataTemperature;
    bmsTemperatureSocData.batterySoc[cntrLoop] = dataSoc;
    /* check validateReadBmsData function */
    runTimeIp = validateReadBmsData(runTimeIpNum,cntrLoop);
    /* when return value is 1, halt requested or max counter value reached */
    if(runTimeIp == 1)
    {
//        printf("breaking the loop \n");
        break;
    }
    else if(runTimeIp == 0)
    {
        /* rewinding the file pointer */
        fseek(fileToBeRead,0,SEEK_SET);
//        rewind(fileToBeRead);
        runTimeIpNum += tempRunTimeIp;
    }
    else
    {
      /* Do nothing */
    }
  }
    bmsTemperatureSocData.numOfData = cntrLoop;
    bmsStatusRet= OK_STATUS;
  return bmsStatusRet;  
}

/*
 **********************************************************************************************************************
 * Service name         : validateReadBmsData
 * Syntax               : int readBmsData(int runTimeIpdata, int cntrLoop)
 * param[in]            : runTimeIpdata
 * param[in]            : cntrLoop
 * return               : 0, 1
 * Description          : Program to verify checks like max count or halt during read operation of Bms data 
 **********************************************************************************************************************
 */
int validateReadBmsData(int runTimeIpdata, int cntrLoop)
{
  int runTimeIp_1 = 0;
  int runTimeIp_2 = 0;
  int retValStatus = 0;
  if(runTimeIpdata == cntrLoop)
  {
    /* entering checkHaltRead function */
    runTimeIp_1 = checkHaltRead();
  }
  runTimeIp_2 = checkStatusRead(runTimeIp_1,runTimeIpdata,cntrLoop);
  retValStatus = runTimeIp_2;
  return retValStatus;
}

/*
 **********************************************************************************************************************
 * Service name         : checkHaltRead
 * Syntax               : int checkHaltRead()
 * param[in]            : none
 * return               : 0, 1
 * Description          : Program to verify and return respective value if halt is requested 
 **********************************************************************************************************************
 */
int checkHaltRead()
{
  int haltInput = 0;
  /* open the user request file */
  FILE * fileCheckHalt= fopen("./Transmitter/BmsReadHalt.txt","r");
    /* check if the opened file is valid or not */
    if(fileCheckHalt)
    {
    /* provide input to the file as 1 to stop the data read else provide 0 */
      while(fscanf(fileCheckHalt, "%d\n", &haltInput)!=EOF)
      {
        ;
      }
    }
    else
    {
      printf("\ncould not open the file\n");
    }
  fclose(fileCheckHalt);
  /* send the updated return value */
  return (haltInput);
}

/*
 **********************************************************************************************************************
 * Service name         : checkStatusRead
 * Syntax               : int checkStatusRead(int runTimeIpStatus,int runTimeIpdata,int cntrLoop)
 * param[in]            : runTimeIpStatus
 * param[in]            : runTimeIpdata
 * param[in]            : cntrLoop
 * return               : 0, 1, 2
 * Description          : Program to verify and return respective value to return based on max count, if halt is 
                          requested or if the requested count and loop counter value is same
 **********************************************************************************************************************
 */
int checkStatusRead(int runTimeIpStatus,int runTimeIpdata,int cntrLoop)
{
  int retValStatus = 2;
  if( (runTimeIpStatus == 1)||(cntrLoop > Max_Count(bmsTemperatureSocData.maxDataToStream)) )
  {
    /* setting return value for checkReadStatus as 1 denoting halt data stream */
    retValStatus = 1;
  }
  else if(runTimeIpdata == cntrLoop)
  {
    /* setting return value for checkReadStatus as 0 denoting rewind the data stream */
    retValStatus = 0;
  }
  else
  {
    /* do nothing */
  }
  return retValStatus;
}
/*
 **********************************************************************************************************************
 * Service name         : dataBmsMain
 * Syntax               : retBmsStatus_en dataBmsMain(void)
 * param[in]            : None
 * return               : OK_STATUS, ERROR_STATUS
 * Description          : Program to invoke reading of required bms datas and send them to console or reciever
 **********************************************************************************************************************
 */
retBmsStatus_en dataBmsMain(int runTimeIpNum, int maxDataStreamRange)
{
  bmsTemperatureSocData.maxDataToStream = maxDataStreamRange;
  retBmsStatus_en retBmsStatus = ERROR_STATUS;
  retBmsStatus = readBmsData(runTimeIpNum);
  if(retBmsStatus != ERROR_STATUS)
  {
    TransmitBmsData();
  }
  return retBmsStatus;
}
