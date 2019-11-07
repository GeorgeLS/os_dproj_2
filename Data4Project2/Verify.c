// Author: A. Delis ad@di.uoa.gr
#include <stdio.h>

#define SIZEofBUFF 20
#define SSizeofBUFF 6

typedef struct{
	long  	custid;
	char 	FirstName[SIZEofBUFF];
	char 	LastName[SIZEofBUFF];
	char	Street[SIZEofBUFF];
	int 	HouseID;
	char	City[SIZEofBUFF];
	char	postcode[SSizeofBUFF];
	float  	amount;
} MyRecord;

int main (int argc, char** argv) {
   FILE *fpb;
   MyRecord rec;
   long lSize;
   int numOfrecords, i;

   if (argc!=2) {
      	printf("Correct syntax is: %s BinaryFile\n", argv[0]);
      	return(1);
   	}
   fpb = fopen (argv[1],"rb");
   if (fpb==NULL) {
      	printf("Cannot open binary file\n");
      	return 1;
   	}

   // check number of records
   fseek (fpb , 0 , SEEK_END);
   lSize = ftell (fpb);
   rewind (fpb);
   numOfrecords = (int) lSize/sizeof(rec);

   // printf("Records found in file %d \n", numOfrecords);
   // sleep(5);

   for (i=0; i<numOfrecords ; i++) {
      	fread(&rec, sizeof(rec), 1, fpb);
      	printf("%ld %s %s  %s %d %s %s %-9.2f\n", \
		rec.custid, rec.LastName, rec.FirstName, \
		rec.Street, rec.HouseID, rec.City, rec.postcode, \
		rec.amount);
   	}

   fclose (fpb);
}
