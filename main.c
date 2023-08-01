#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <zip.h>
#include <libxml/parser.h>

#define CHECK(retval, errval, errmsg, handle)  { \
  if(retval == errval){ \
    fprintf(stderr, "Runtime error: %s at %s:%d.\n", errmsg, __FILE__, __LINE__); \
    handle; \
  } \
}

typedef enum{
  TRUE,
  FALSE
}BOOL;

int openArchive(char** documentContent, const char* filename){
  const char* documentXML = "word/document.xml";
  int archiveFileIndex;
  zip_t* archive; 
  struct zip_file* archiveFile;
  struct zip_stat archiveStat;

  int err;
  archive = zip_open(filename, ZIP_RDONLY, &err);
  CHECK(archive, NULL, "Failed to open archive", 
      fprintf(stderr, "Error: Unable to open file %s\n", filename); return -1;)

  archiveFileIndex = zip_name_locate(archive, documentXML, 0);
  CHECK(archiveFileIndex, -1, "Failed to locate file in archive",
      fprintf(stderr, "Error: Unable to locate file %s in archive %s\n", documentXML, filename); return -1;)

  zip_stat_index(archive, archiveFileIndex, 0, &archiveStat);

  archiveFile = zip_fopen_index(archive, archiveFileIndex, 0);

  *documentContent = calloc(archiveStat.size, sizeof(char));
  CHECK(documentContent, NULL, "Failed to allocate memory", return -1;)

  err = zip_fread(archiveFile, *documentContent, archiveStat.size);
  CHECK(err, -1, "Failed to read file", return -1;)

  zip_discard(archive);
  return 0;
}

int main(int argc, char** argv){
  int FILEOUT = FALSE;
  int FILEARCHIVE = FALSE;
  char* archiveName;
  char* archiveContent;
  char* fileOut;

  int opt;
  do {
    switch(opt = getopt(argc, argv, "o:")){
      case 'o':
        if(optarg != NULL){
          FILEOUT = TRUE;
          fileOut = calloc(strlen(optarg), sizeof(char));
          strcpy(fileOut, optarg);
        }
        break;

      case -1:
        if(argv[optind] != NULL){
          FILEARCHIVE = TRUE;
          archiveName = calloc(strlen(argv[optind]), sizeof(char));
          strcpy(archiveName, argv[optind]);
        }
        break;

      case '?':
        fprintf(stderr, "Unknown option: %c\n", optopt);
        return 0;
    }
  }while(opt >= 0);

  if(FILEARCHIVE == FALSE){
    fprintf(stderr, "Require one input file");
    if(FILEOUT == TRUE){ free(fileOut); }
    return 0;
  }


  openArchive(&archiveContent, archiveName);

  if(FILEOUT == TRUE){
    FILE* fd = fopen(fileOut, "w");
    fprintf(fd, archiveContent);
  }


  if(FILEOUT == TRUE){ free(fileOut); }
  if(FILEOUT == TRUE){ free(archiveName); }
  free(archiveContent);
  return 0;
}
