#include "../../header/LTMRecorder/LTMRecorder.h"


namespace LLib
{
	namespace IO
	{


		LTMRecorder::LTMRecorder()
		{
			this->fp = NULL;
			this->pTMatrix = NULL;
			this->fileStatus = FILESTATUS_NOFILE;
		}

		LTMRecorder::~LTMRecorder()
		{
		}

// write the matrix value to file
		void LTMRecorder::startRecord(char * fname)
		{
			if (this->pTMatrix == NULL)
			{
				fprintf( stderr, "LTMRecorder::startRecord Matrix don't be set!\n");
				return ;
			}
			if( openFile(fname, this->FILESTATUS_RECORD) )
				return ;	// non-zero value means some error happen.

		}
		void LTMRecorder::endRecord()
		{
			closeFile();
		}
		void LTMRecorder::recordMatrixValue()
		{
			if (this->pTMatrix == NULL)
			{
				fprintf( stderr, "LTMRecorder::recordMatrixValue Matrix don't be set!\n");
				return ;
			}


		}
// load the file value to matrix
		void LTMRecorder::loadRecordFile(char * fname)
		{
			// open recorder file.
			if( openFile(fname, this->FILESTATUS_PLAY) )
			{
				return ;	// non-zero value means some error happen.
			}
			// load the values to memory.



		}

		void LTMRecorder::updateMatrixValue()
		{
			if (this->pTMatrix == NULL)
			{
				fprintf( stderr, "LTMRecorder::updateMatrixValue Matrix don't be set!\n");
				return ;
			}

		}


//	private:
		int LTMRecorder::openFile(char * fname, int status)
		{
			if (fp!=NULL)
			{
				fprintf( stderr, "LTMRecorder:: file opened, you should close file first!\n");
				return 1;
			}

			switch( status) 
			{
			case this->FILESTATUS_PLAY:
				if (NULL == (fp = fopen(fname,"r")))
				{
					fprintf( stderr, "LTMRecorder::openFile file\"%s\" open fail, you should close file first!\n", fname);
					return 1;
				}
				this->fileStatus = this->FILESTATUS_PLAY;
				break;
			case this->FILESTATUS_RECORD:
				if (NULL == (fp = fopen(fname,"w")))
				{
					fprintf( stderr, "LTMRecorderopenFile file\"%s\" open fail, you should close file first!\n", fname);
					return 1;
				}
				this->fileStatus = this->FILESTATUS_RECORD;
				break;
			default:
				fprintf( stderr, "LTMRecorder::openFile, wrong value of status parameter is passed.\n");
				return 1;
			}
			return 0;
		}

		int LTMRecorder::closeFile()
		{
			if (fp!=NULL)
			{
				fclose(fp);
				fp = NULL;
				this->fileStatus = this->FILESTATUS_NOFILE;
			}
			return 0;
		}

		inline void LTMRecorder::writeMatrix()
		{
		}

		inline void LTMRecorder::readMatrix()
		{
		}



	}
}
