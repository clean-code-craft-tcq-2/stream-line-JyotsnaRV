#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "test/catch.hpp"
#include "Transmitter/bmsDataTransmitter.h"


/*
Test case Description:
1. The first argument of dataBmsMain function is a value which is used to check every nth count, if user has requested for Halt or not.
2. if the user has requested for a halt(when 1 is requested), the first nth count of 1st argument value is enough to break lopp and stop streaming.
3. The second argument of dataBmsMain function is a value that consists of max count the data should stream, when user does not request halt(when 0 is requested).
*/
//TEST_CASE(" Test case to check if data is read and sender sends data ") 
//{
 /* This will stream datas in multiples of 100 */
 //int runTimeIpNum_u32 = 50; /* Every 50th count of loop will enter into user request check to Halt the read or not */
 //int maxStreamingRange = 20; /* this input will be taken as multiples of 5 */
 //REQUIRE(dataBmsMain((runTimeIpNum_u32-1), maxStreamingRange) == OK_STATUS);
//}

TEST_CASE(" Test case to check if user request to halt the read after (100-1) as max count ") 
{
 /* Note: haltBmsRead.txt contains value as 0 in this case */
 /* This will stream only 100 datas */
 int runTimeIpNum_u32 = 50; /* Every 100th count of loop will enter into user request check to Halt the read or not */
 int maxStreamingRange = 50; /* this input will be taken as multiples of 5 */
 REQUIRE(dataBmsMain((runTimeIpNum_u32-1),maxStreamingRange) == OK_STATUS);
}

TEST_CASE(" Test case to check if user request to halt the read after (10-1) as max count ") 
{
 // Note: haltBmsRead.txt contains value as 0 in this case, so change in file as 1 and try the test case
 // This will stream only 10 datas, since it will perform a user request check if user wants to halt the streaming of data
 int runTimeIpNum_u32 = 10; 
 int maxStreamingRange = 10;
 REQUIRE(dataBmsMain((runTimeIpNum_u32-1),maxStreamingRange) == OK_STATUS);
}
