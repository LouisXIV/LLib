namespace LLib
{
	namespace IO
	{

		class LTMRecorder;
	}
}


#ifndef _LTMRECORDER_H_
#define _LTMRECORDER_H_


#include <stdio.h>
#include <stdlib.h>
#include "../LTranslateMatrix/LTranslateMatrix.h"
using LLib::Math::LTranslateMatrix;


namespace LLib
{
	namespace IO
	{

		class LTMRecorder
		{
		public:
		protected:
		private:
			LLib::Math::LTranslateMatrix * pTMatrix;
			FILE * fp;
			int fileStatus;	// 0: no file set, 1: file to play, 2: file to record.
			static const int FILESTATUS_NOFILE	= 0;
			static const int FILESTATUS_PLAY	= 1;
			static const int FILESTATUS_RECORD	= 2;
						// this value is maintained by "openFile(), closeFile(), included initialization.

		public:
			LTMRecorder();
			~LTMRecorder();

			void setMatrix(LLib::Math::LTranslateMatrix * ptm)	{ this->pTMatrix = ptm; return;};

			void startRecord(char * fname);
			void endRecord();
			void recordMatrixValue();

			void loadRecordFile(char * fname);
			void updateMatrixValue();


//			void check();	// check is the recorder ready to work!! 1. file opened, 2. matrix gained.

		protected:

		private:
			int openFile(char * fname, int status);
			int closeFile();
			inline void writeMatrix();
			inline void readMatrix();

		};

	}	// namespace IO
}		// namespace LLib





#endif // _LTMRECORDER_H_