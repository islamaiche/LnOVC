#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <conio.h>
#define  max  1020
#define max2  888888


///********************************************************
///***************** DECLARATION DES TYPES ****************
///********************************************************

typedef struct bloc{
	char chainne[max];
	int suivant;
}bloc;

typedef struct bloc Buffer;

typedef struct entete{   /// structure de l'entete du fichier en mémoire centrale
    int nbbloc;
    int tete;
    int queue;
    int indice_libre;    /// l'indice libre dans le bloc de la queue
}entete;

typedef struct LnOVC{
    FILE *fich;
    entete entete;
}LnOVC;

typedef struct cord{
	char matricule[7];
	char nom[31];
	char prenom[31];
	char date_de_naissance[11];
	char wilaya_de_naissance[20];
	char groupe_sanguin[4];
	char grade[30];
	char force_armee[50];
	char regio_militaire[20];
}cord;

typedef struct index{
	int cle;
	int numero_de_bloc;
	int position;
}index;

///*********************************************************
///******************** VARIABLE GLOBALE *******************
///*********************************************************

int var;
char nomfich 	  [30]="PERSONNEL-ANP_DZ.bin";
char fichier_index[30]="fichier_index.txt";
char force_arm 	  [30]="force_arm.txt";
char region_mili  [30]="region militaire.txt";
char car_grade    [30]="categorie de grades.txt";	
long nombre_de_liredir=0,nombre_de_ecriredir=0;
FILE *file_wilaya;
FILE *file_region;
FILE *file_grade;
FILE *file_force;

///*********************************************************
///*********************************************************
///*********************************************************

void Liredir	(LnOVC *fichier, int i, Buffer *buf);
void Ecriredir	(LnOVC *fichier, int i, Buffer *buf);
int  Entete 	(LnOVC *fichier, int i );
void Aff_Entete (LnOVC *fichier, int i, int valeur);
LnOVC *ouvrir	(char nom_physique[], char mode);
void fermer 	(LnOVC *fichier);
void alloc_bloc (LnOVC *fichier);
int  alea 		(int a,int b);					// donnez un valeur aleatoir entre a et b
void ins_nom	(char mot[31]);					// initialiser le nom/prenom
void ins_ddn	(char dnn[11]);					// initialiser la date de naissance
void ins_wil	(char wilaya[20]);				// initialiser la wilaya de naissance
void ins_gs		(char gs[4]);					// initialiser le groupe sanguin
void ins_grade	(char grade[30],FILE *f1,long matricule);				// initialiser le grade
void ins_force	(char force[50],FILE *f,long matricule);// initialiser le type de force
void ins_reg	(char region[20],FILE *f1,long matricule);				// inisialiser la region
void chargement_initial(cord *cor,FILE *f,FILE *f2,FILE *f3);		// initialiser l enrigistrement
void get_new_enr(cord *cor);
void ins_fich 	(FILE *f);
void ins_fich2 	(FILE *f);
void cordcpy	(char chainne[210],cord cor);	// copy l enregistrement dans une chainne 
void new_ligne 	(FILE *f);						// se positioner a debut de la ligne suivante
void get_ligne	(FILE *f,char cordo[30]);		// avoir une ligne a partire du fichier d'index
long get_mat 	(FILE *f);						// avoir le matricule a partire du fichier index
void ecrire     (FILE *f,int i,long matricule);
long lire   	(FILE *f,int sup);
void supprimer  (FILE *f,long  matricule);
void check_place(FILE *f,char cordo[30],char matric[7],int *trouv);// ecrire les cordonner dans le fichier d'index d'une facon trie
void reche_place(char cordo[30],long matricule,int *trouv);
void mod_place	(char cordo[30],char matric[7]);// modifier cordoner dans le fichier d'index
void creation_epuration();						// j ai utiliser un seul module just pour optimiser  
void ins_new	();     					    // insertion d'un nouvel enregistrement
void mod_rm		();						   	    // modifier la region milittaire
void supp_tt_force();
void menu();
void check_choix();
///*********************************************************
///******************* MACHINE ABSTRET *********************
///*********************************************************
// positionnement au debut du bloc numero i
// lecture d'un bloc de caractère correspondant a la taille du bloc dans le buffer
// repositionnement endebut de fichier

void Liredir(LnOVC *fichier, int i , Buffer *buf){
	nombre_de_liredir++;
 	fseek(fichier->fich,(sizeof(entete)+sizeof(bloc)*(i-1)),SEEK_SET);
 	fread(buf,sizeof(Buffer),1,fichier->fich);                         
 	rewind(fichier->fich);                                             
}

void Ecriredir(LnOVC *fichier, int i, Buffer *buf){
	nombre_de_ecriredir++;
    fseek(fichier->fich,sizeof(entete)+sizeof(bloc)*(i-1),SEEK_SET); // positionnement au debut du bloc numero i
    fwrite(buf,sizeof(Buffer),1,fichier->fich);                     //ecriture du contenu du buffer dans le bloc numero i du fichier
}

int Entete(LnOVC *fichier, int i){
    switch(i)
    {
        case 1:{
            return(fichier->entete.nbbloc 		);
        }break;
        case 2:{
            return(fichier->entete.tete 		);
        }break;
        case 3:{
            return(fichier->entete.queue		);
        }break;
        case 4:{
            return(fichier->entete.indice_libre );// la postion libre dans le bloc de queue
        }break;
    };
}

void Aff_Entete(LnOVC *fichier, int i, int valeur){
    switch(i){
        case 1:{
            fichier->entete.nbbloc=valeur;		 // nombre total de bloc alloués dans le fichier
        }break;
        case 2:{
            fichier->entete.tete=valeur  ;		 // numero du bloc representatnt la tete du fichier
        }break;
        case 3:{
            fichier->entete.queue=valeur ;		 // numero du bloc representatnt la queue du ficheir
        }break;
        case 4:{
            fichier->entete.indice_libre=valeur; // la postion libre dans le bloc de queue
        }break;
    };
}

LnOVC *ouvrir(char nom_physique[], char mode){
    LnOVC *fichier=malloc(sizeof(LnOVC));                      // allocation  de la structure
    Buffer buf;
    if( (mode=='A') ||  (mode == 'a') ){                       // mode ancien
        fichier->fich=fopen(nom_physique,"rb+");               // ouverture du fichier en mode binaire lecture et ecriture
        fread(&(fichier->entete),sizeof(entete),1,fichier->fich);  // chargement de l'entete enregistrée en debut de fichier    
    }
    else{
        if( (mode=='N') ||  (mode == 'n') ){                 // mode nouveau
            fichier->fich=fopen(nom_physique,"wb+");         // ouverture du fichier en mode binaire  ecriture
            Aff_Entete(fichier,1,0);                         // mise a zeo du nombre de bloc si entete(fichier,1)=0 alors le fichier est vide
            Aff_Entete(fichier,2,1);                         // mettre tete au premier bloc
            Aff_Entete(fichier,3,1);                         // de meme pour la queue puisque au debut tete est queue sont confondues
            Aff_Entete(fichier,4,0);                         // le premier caractère du ficheir correspond a la position libre puisqu'il est nouveau
            Aff_Entete(fichier,5,0);                         // aucun caractère n'a encore été supprimé
            fwrite(&(fichier->entete),sizeof(entete),1,fichier->fich);// enregistrement de l'entete dans le fichier
            buf.suivant=-1;                                  // le suivant du premier bloc a NULL
            sprintf(buf.chainne,"%s","");                    // initialisation du buffer a chaine vide
            Ecriredir(fichier,1,&buf);                       // ecriture du premier bloc dans le fichier
        }
        else{                                                // format d'ouverture incorrecte
            printf("format d'ouverture impossible");
        }
    }
    return(fichier);                                         // renvoyer la structure créée
}

void fermer(LnOVC *fichier){  // procedure de fermeture du fichier
	entete c;
    rewind(fichier->fich); // repositionnement du curseur en debut de fichier
    fwrite(&(fichier->entete),sizeof(entete),1,fichier->fich); // sauvegarde de la dernière version de l'entete de la strucuture L7OVC
    rewind(fichier->fich);// repositionnement du curseur en debut de fichier
    fclose(fichier->fich);// fermeture du fichier
}

//--permet d'allouer un nouveau bloc dans le ficiher et de mettre a jour les caractéristiques des bloc et de l'entete touchés--//

void  alloc_bloc(LnOVC *fichier){
        Buffer *buf=malloc(sizeof(Buffer));       // allocation du Buffer
        Liredir(fichier,Entete(fichier,3),buf);   // lecture du bloc correspondant a la queue
        buf->suivant=Entete(fichier,1)+1;         // mise a jour dui suvant de la queue au bloc correspondant a la nouvelle queue
        Ecriredir(fichier,Entete(fichier,3),buf); // ecriture du bloc de queue dans le fichier
        Aff_Entete(fichier,3,Entete(fichier,1)+1);// mise a jour du numero du bloc correspondant a la nouvelle queue dan sl'entete
        buf->suivant=-1;                          // mise a jour du suivant a nill
        sprintf(buf->chainne,"%s","");            // vider la chaine du buffer
        Ecriredir(fichier,Entete(fichier,3),buf); // ecriture du buffer dans le bloc representatnt la nouvelle queue
        Aff_Entete(fichier,1,Entete(fichier,1)+1);// incrémentation du nombre de bloc alloués
}


///*********************************************************
///***************** PARTIE INITIALISATION *****************
///*********************************************************

///cette fonction retourne un nombre aleatoir entre a et b
int alea(int a,int b){
	return (rand()%(b-a))+a;
}

///le role de ce module est d initialiser le nom/prenom
void ins_nom(char mot[31]){
	int i=0,a=alea(4,30);
	mot[i]=alea(65,90);
	for (i = 1; i < a; i++)
	{
		mot[i]=alea(97,122);
	}
	mot[i]='\0';
}

///le role de ce module est d initialiser la date de naissance
void ins_ddn(char dnn[11]){
	int annee=alea(1940,1999);
	int mois=alea(1,12);
	int jour,i=0;
	if (mois==4||mois==6||mois==9||mois==11){
		jour=alea(1,30);
	}else if(mois==2){
		if(annee % 4 ==0 && annee % 100 !=0){
			jour=alea(1,29);
		}else jour=alea(1,28);			
	}else jour=alea(1,31);
	if(jour<10){
		if(mois<10){
			sprintf(dnn,"0%d/0%d/%d",jour,mois,annee);
		}else{
			sprintf(dnn,"0%d/%d/%d",jour,mois,annee);
		}
	}else{
		if(mois<10){
			sprintf(dnn,"%d/0%d/%d",jour,mois,annee);
		}else{
			sprintf(dnn,"%d/%d/%d",jour,mois,annee);
		}
	}
}

///le role de ce module est d initialiser la wilaya
void ins_wil(char wilaya[20]){
	int i=0,matric=alea(0,48);
	fseek(file_wilaya,21*matric,SEEK_SET);
	char c=fgetc(file_wilaya);
	while(c!='.'){
		wilaya[i]=c;
		c=fgetc(file_wilaya);
		i++;
	}
	wilaya[i]='\0';
	
}

///le role de ce modle est d initialiser le groupe sansguin
void ins_gs(char gs[4]){
	int i=alea(1,8);
	if(i==8){
		strcpy(gs,"AB-");
	}else if(i==1){
		strcpy(gs,"O+");
	}else if(i==2){
		strcpy(gs,"A+");
	}else if(i==3){
		strcpy(gs,"B+");
	}else if(i==4){
		strcpy(gs,"O-");
	}else if(i==5){
		strcpy(gs,"A-");
	}else if(i==6){
		strcpy(gs,"AB+");
	}else if(i==7){
		strcpy(gs,"B-");
	}
	
}

///le role de modeile est d initialiser le grade
void ins_grade(char grade[30],FILE *f1,long matricule){
	int i,gra=alea(1,18);
	if 		(gra<4 ){i=1;}
	else if (gra<7 ){i=2;}
	else if (gra<11){i=3;}
	else if (gra<15){i=4;}
	else 			{i=5;}
	if (var==0){ecrire(f1,i,matricule);}
	fseek(file_grade,27*(gra-1),SEEK_SET);
	i=0;
	char c=fgetc(file_grade);
	while(c!='.'){
		grade[i]=c;
		c=fgetc(file_grade);
		i++;
	}
	grade[i]='\0';
} 

///le role de ce module est de donne de quelle force 
void ins_force(char force[50],FILE *f2,long matricule){
	int i=0,nombre=alea(1,9);
	if (var==0){ecrire(f2,nombre,matricule);}
	fseek(file_force,49*(nombre-1),SEEK_SET);
	char c=fgetc(file_force);
	while(c!='.' ){
		force[i]=c;
		c=fgetc(file_force);
		i++;
	}
	force[i]='\0';
}

///le role de ce module est de donne la region
void ins_reg(char region[20],FILE *f1,long matricule){
	int i=0,nombre=alea(1,7);
	if (var==0){ecrire(f1,nombre,matricule);}
	fseek(file_region,18*(nombre-1),SEEK_SET);
	char c=fgetc(file_region);
	while(c!='.'){
		region[i]=c;
		c=fgetc(file_region);
		i++;
	}
	region[i]='\0';
}

void chargement_initial(cord *cor,FILE *f,FILE *f2,FILE *f3){
	sprintf(cor->matricule,"%d%d",alea(111,999),alea(111,999));
	ins_nom(cor->nom);
	ins_nom(cor->prenom);
	ins_ddn(cor->date_de_naissance);
	ins_wil(cor->wilaya_de_naissance);
	ins_gs(cor->groupe_sanguin);
	ins_grade(cor->grade,f3,atol(cor->matricule));
	ins_force(cor->force_armee,f,atol(cor->matricule));
	ins_reg(cor->regio_militaire,f2,atol(cor->matricule));
}
///******************************************************************
///******************************************************************
///******************************************************************


void cordcpy(char chainne[210],cord cor){
	int i=10+strlen(cor.matricule)+strlen(cor.nom)+strlen(cor.prenom)+strlen(cor.date_de_naissance)+strlen(cor.wilaya_de_naissance)+strlen(cor.groupe_sanguin)+strlen(cor.grade)+strlen(cor.force_armee)+strlen(cor.regio_militaire);
	sprintf(chainne,"%d#%s#%s#%s#%s#%s#%s#%s#%s#%s#",i,cor.matricule,cor.nom,cor.prenom,cor.date_de_naissance,cor.wilaya_de_naissance,cor.groupe_sanguin,cor.grade,cor.force_armee,cor.regio_militaire);
}

void cpycord(char chainne[210],cord *cor){
	char *mot;
	int cpt=0;
	mot=strtok(&chainne[cpt],"#");
	cpt+=strlen(mot)+1;
	strcpy(cor->matricule,mot);
	mot=strtok(&chainne[cpt],"#");
	cpt+=strlen(mot)+1;
	strcpy(cor->nom,mot);
	mot=strtok(&chainne[cpt],"#");
	cpt+=strlen(mot)+1;
	strcpy(cor->prenom,mot);
	mot=strtok(&chainne[cpt],"#");
	cpt+=strlen(mot)+1;
	strcpy(cor->date_de_naissance,mot);
	mot=strtok(&chainne[cpt],"#");
	cpt+=strlen(mot)+1;
	strcpy(cor->wilaya_de_naissance,mot);
	mot=strtok(&chainne[cpt],"#");
	cpt+=strlen(mot)+1;
	strcpy(cor->groupe_sanguin,mot);
	mot=strtok(&chainne[cpt],"#");
	cpt+=strlen(mot)+1;
	strcpy(cor->grade,mot);
	mot=strtok(&chainne[cpt],"#");
	cpt+=strlen(mot)+1;
	strcpy(cor->force_armee,mot);
	mot=strtok(&chainne[cpt],"#");
	cpt+=strlen(mot)+1;
	strcpy(cor->regio_militaire,mot);
	//printf("\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s",cor->matricule,cor->nom,cor->prenom,cor->date_de_naissance,cor->wilaya_de_naissance,cor->groupe_sanguin,cor->grade,cor->force_armee,cor->regio_militaire);
}

void get_new_enr(cord *cor){
	int annee=1900,mois=100,jour=100;
	printf("Nom : ");
	scanf("%s",&cor->nom);
	printf("Prenom : ");
	scanf("%s",&cor->prenom);
	printf("Date de naissance : \n");
	while(annee<1940 || annee >1999){
		printf("    annee de naissance : ");
		scanf("%d",&annee);
	}
	while(mois<1 || mois>12){
		printf("    mois de naissance : ");
		scanf("%d",&mois);
	}
	if (mois==4||mois==6||mois==9||mois==11){
		while(jour<1 || jour>30){
			printf("    jour de naissance : ");
			scanf("%d",&jour);
		}
	}else if(mois==2){
		if(annee % 4 ==0 && annee % 100 !=0){
			while(jour<1 || jour>29){
				printf("    jour de naissance : ");
				scanf("%d",&jour);
			}
		}else{
			while(jour<1 || jour>28){
				printf("    jour de naissance : ");
				scanf("%d",&jour);
			}
		}			
	}else{
		while(jour<1 || jour>31){
			printf("    jour de naissance : ");
			scanf("%d",&jour);
		}
	}
	if(jour<10){
		if(mois<10){
			sprintf(cor->date_de_naissance,"0%d/0%d/%d",jour,mois,annee);
		}else{
			sprintf(cor->date_de_naissance,"0%d/%d/%d",jour,mois,annee);
		}
	}else{
		if(mois<10){
			sprintf(cor->date_de_naissance,"%d/0%d/%d",jour,mois,annee);
		}else{
			sprintf(cor->date_de_naissance,"%d/%d/%d",jour,mois,annee);
		}
	}
	printf("Wilaya de naissance : \n");
	printf("   01.Adrar         \t02.Chlef         \t03.Laghouat  \t04.Oum El Bouaghi    \t05.Batna\n");
	printf("   06.Bejaia        \t07.Biskra        \t08.Bechar    \t09.Blida             \t10.Bouira\n");
	printf("   11.Tamanrasset   \t12.Tebessa       \t13.Tlemcen   \t14.Tiaret            \t15.Tizi Ouzou\n");
	printf("   16.Alger         \t17.Djelfa        \t18.Jijel     \t19.Setif             \t20.Saida\n");
	printf("   21.Skikda        \t22.Sidi Bel Abbes\t23.Annaba    \t24.Guelma            \t25.Constantine\n");
	printf("   26.Medea         \t27.Mostaganem    \t28.M Sila    \t29.Mascara           \t30.Ouargla\n");
	printf("   31.Oran          \t32.El Bayadh     \t33.Illizi    \t34.Bordj Bou Arreridj\t35.Boumerdes\n");
	printf("   36.El Tarf       \t37.Tindouf       \t38.Tissemsilt\t39.El Oued		     \t40.Khenchela\n");
	printf("   41.Souk Ahras    \t42.Tipaza	       \t43.Mila      \t44.Ain Defla        \t45.Naama\n");
	printf("   46.Ain Temouchent\t47.Ghardaia      \t48.Relizane\n");
	printf("entre  le matricule de la wilaya\n");
	scanf("%d",&jour);
	mois=1;
	FILE *f=fopen("wilaya.txt","r");
	char c=fgetc(f);
	while(mois<jour){
		if (c=='\n'){mois++;}
		c=fgetc(f);
	}
	mois=0;
	while(c!='\n' && c!=EOF){
		cor->wilaya_de_naissance[mois]=c;
		c=fgetc(f);
		mois++;
	}
	cor->wilaya_de_naissance[mois]='\0';
	fclose(f);
	printf("Groupe sanguin : \n");
	printf("           1.O+  2.A+  3.B+  4.O-  5.A-  6.AB+  7.B-  8.AB-\n");
	scanf("%d",&jour);
	if(jour==8){
		strcpy(cor->groupe_sanguin,"AB-");
	}else if(jour==1){
		strcpy(cor->groupe_sanguin,"O+");
	}else if(jour==2){
		strcpy(cor->groupe_sanguin,"A+");
	}else if(jour==3){
		strcpy(cor->groupe_sanguin,"B+");
	}else if(jour==4){
		strcpy(cor->groupe_sanguin,"O-");
	}else if(jour==5){
		strcpy(cor->groupe_sanguin,"A-");
	}else if(jour==6){
		strcpy(cor->groupe_sanguin,"AB+");
	}else if(jour==7){
		strcpy(cor->groupe_sanguin,"B-");
	}
	printf("Grade : \n");
	printf("01.General de corps d armee\t02.General-Major\t03.General\t04.Colonel\n05.Lieutenant-colonel       \t06.Commandant        \t07.Capitaine\t08.Lieutenant\n09.Sous-lieutenant        \t10.Aspirant        \t11.Adjudant-chef\t12.Adjudant\n13.Sergent-chef   \t14.Sergent\t15.Caporal-chef    \t16.Caporal\t17.Djoundi\n" );
	jour=1;
	scanf("%d",&mois);
	f=fopen(car_grade,"r+");
	if 		(mois<4 ){jour=1;}
	else if (mois<7 ){jour=2;}
	else if (mois<11){jour=3;}
	else if (mois<15){jour=4;}
	else 			 {jour=5;}
	ecrire(f,jour,atol(cor->matricule));
	fclose(f);
	jour=1;
	f=fopen("grade.txt","r");
	c=fgetc(f);
	while(jour<mois){
		if (c=='\n'){jour++;}
		c=fgetc(f);
	}
	jour=0;
	while(c!='\n' && c!=EOF){
		cor->grade[jour]=c;
		c=fgetc(f);
		jour++;
	}
	cor->grade[jour]='\0';
	fclose(f);
	printf("Force_armee : \n");
	printf("01.Armee de terre        \t02.Armee de l air\n03.Marine nationale        \t04.Defense aerienne du territoire\n05.Gendarmerie nationale  \t06.Garde republicaine \n07.Departement du renseignement et de la securite\t08.Sante militaire\n");	
	mois=1;
	scanf("%d",&jour);
	f=fopen(force_arm,"r+");
	ecrire(f,jour,atol(cor->matricule));
	fclose(f);
	f=fopen("force.txt","r");
	c=fgetc(f);
	while(mois<jour){
		if (c=='\n'){mois++;}
		c=fgetc(f);
	}
	mois=0;
	while(c!='\n' && c!=EOF){
		cor->force_armee[mois]=c;
		c=fgetc(f);
		mois++;
	}
	cor->force_armee[mois]='\0';
	fclose(f);
	printf("Regio_militaire : \n");
	printf("   1RM-Blida\t2RM-Oran\t3RM-Bechar\n   4RM-Ouargla\t5RM-Constantine\t6RM-Tamanrasset\n");

	mois=1;
	scanf("%d",&jour);
	f=fopen(region_mili,"r+");
	ecrire(f,jour,atol(cor->matricule));
	fclose(f);
	f=fopen("region.txt","r");
	c=fgetc(f);
	while(mois<jour){
		if(c=='\n'){mois++;}
		c=fgetc(f);
	}
	mois=0;
	while(c!='\n' && c!=EOF){
		cor->regio_militaire[mois]=c;
		c=fgetc(f);
		mois++;
	}
	cor->regio_militaire[mois]='\0';
	fclose(f);
}

///******************************************************************
///******************************************************************
///******************************************************************


void new_ligne(FILE *f){
	char c=fgetc(f);
	while(c!=EOF && c!='\n'){
		c=fgetc(f);
	}
}

void get_ligne(FILE *f,char cordo[30]){
	char c=fgetc(f);
	int i=0;
	while(c!=EOF && c!='\n'){
		cordo[i]=c;
		c=fgetc(f);
		i++;
	}
	cordo[i]='\0';
}

long get_mat(FILE *f){
	char matricule[7];
	fgets(matricule,7,f);
	fseek(f,-6,SEEK_CUR);
	return atol(matricule);
}

void ins_fich(FILE *f){
	long i;
	for(i=111111;i<=999999;i++){
		fprintf(f, "%d                      \n", i);
	}
}

void ins_fich2(FILE *f){
	long i;
	fprintf(f,"0     0     0     0     0     0     0     0     \n");
	for(i=1;i<=50000;i++){
	fprintf(f,"                                                \n");
	}
}

 
void ecrire(FILE *f,int i,long matricule){
	char place[10];
	fseek(f,6*(i-1),SEEK_SET);
	fgets(place,7,f);
	fseek(f,-6,SEEK_CUR);
	fprintf(f,"      " );
	fseek(f,-6,SEEK_CUR);
	fprintf(f,"%d",atol(place)+1);
	fseek(f,6*(i-1),SEEK_SET);
	fseek(f,50*(atol(place)+1),SEEK_CUR);
	fprintf(f,"%d",matricule );
}


long lire(FILE *f,int sup){//sup est un bool donc si sup=1 a chaque lecrure il va suprimier
	char matric[10];
	fgets(matric,7,f);
	if(sup==1){
		fseek(f,-6,SEEK_CUR);
		fprintf(f,"      ");
	}
	fseek(f,44,SEEK_CUR);	
	return atol(matric);
}


void check_place(FILE *f,char cordo[30],char matric[7],int *trouv){
	long matricule,i;
	char ligne[30]="",c;
	matricule=atol(matric);
	fseek(f,30*(matricule-111111),SEEK_SET);
	fseek(f,6,SEEK_CUR);
	c=fgetc(f);
	if(c==' '){
		fseek(f,(-1),SEEK_CUR);//printf("%s\n",matric );
		fprintf(f,"%s",cordo );
	}else{
		*trouv=1;//printf("%s*******\n",matric );
	}	
}

void reche_place(char cordo[30],long matricule,int *trouv){
	char ligne[30]="",c;
	FILE *f=fopen(fichier_index,"r");
	fseek(f,30*(matricule-111111),SEEK_SET);
	fseek(f,6,SEEK_CUR);
	c=fgetc(f);
	if(c==' '){
		*trouv=0;
		//printf("le matricule n'existe pas.\n");
	}else{
		fgets(cordo,15,f);
		*trouv=1;
	}	
	fclose(f);
}

void supprimer(FILE *f,long matricule){
	fseek(f,30*(matricule-111111),SEEK_SET);
	fseek(f,6,SEEK_CUR);
	fprintf( f , "                 " );
}

int get_enr(cord *cor,long matricule){
	int i=0,j,h,g,trouv;
	Buffer *buf=malloc(sizeof(Buffer));
	LnOVC *fichi=ouvrir(nomfich,'a');
	char chainne[220],cordo[30],*debut;
	reche_place(cordo,matricule,&i);
	trouv=i;
	if(i==1){
		i=strlen(strtok(cordo,"/"))+1;
		h=atol(strtok(cordo,"/"));
		Liredir	(fichi,h,buf);
		debut=strtok(&cordo[i]," ");
		j=atol(debut);
		if(1019-j>4){
			strncpy(cordo,&buf->chainne[j],4);
			cordo[4]='\0';
			i=atol(strtok(cordo,"#"));
			j+=strlen(strtok(cordo,"#"))+1;
		}else{
			strncpy(cordo,&buf->chainne[j],1019-j);
			buf->chainne[0]='\0';
			Liredir(fichi,h+1,buf);
			strncpy(&cordo[1019-j],buf->chainne,-1015+j);
			cordo[4]='\0';
			i=atol(strtok(cordo,"#"));
			if(buf->chainne[0]!='#'){
				j=strlen(strtok(buf->chainne,"#"))+1;
			}else{
				j=1;
			}
		}		
		if(1020-j>i-1){
			strncpy(chainne,&buf->chainne[j],i-1);
			chainne[i-1]='\0';
		}else{
			g=1019-j;
			strncpy(chainne,&buf->chainne[j],g);
			buf->chainne[0]='\0';
			Liredir(fichi,h+1,buf);
			strncpy(&chainne[g],buf->chainne,i-g-1);
			chainne[i-1]='\0';
		}
		cpycord(chainne,cor);
	}
	fermer(fichi);
	return trouv;
}

//le role de ce module est de calculer l age
int get_age(cord cor){
	char *date=strtok(&cor.date_de_naissance[3],"/");
	if(atol(date)==1){
		return 2018-atol (&cor.date_de_naissance[6]/*[4+strlen(date)]*/);
	}else{
		return 2017-atol (&cor.date_de_naissance[6]);
	}
}

void creation_epuration(){
	int i,j,N,cpt,nombre_de_bloc=1,trouv=0,nombre_de_enrigistrement_effacer=0;     //N enregistrements 
	char chainne[210];
	Buffer *buf=malloc(sizeof(Buffer));
	cord cor;
	char cordo[30];
	LnOVC *fichi=ouvrir(nomfich,'N');
	FILE *f =fopen(fichier_index,"w");
	ins_fich(f);
	fclose(f);
	f       = fopen( fichier_index , "r+" );
	FILE *f1= fopen( force_arm 	   , "w+" );
	FILE *f2= fopen( region_mili   , "w+" );
	FILE *f3= fopen( car_grade 	   , "w+" );
	ins_fich2(f1);
	ins_fich2(f2);
	ins_fich2(f3);
	printf("Combien d enregistrements voulez vous ajouter : N = ");
	scanf("%d",&N);
	printf("ATENTION: cette operation peux prendre des minutes...\n");
	alloc_bloc(fichi);
	Liredir(fichi,nombre_de_bloc,buf);
	for(i=0;i<N;i++){
		var=0;
		chargement_initial(&cor,f1,f2,f3);
		sprintf(cordo,"/%d/%d\0",nombre_de_bloc,strlen(buf->chainne) );
		cordo[strlen(cordo)]='\0';
		check_place(f,cordo,cor.matricule,&trouv);
		if(trouv==1){nombre_de_enrigistrement_effacer++;var=1;trouv=0;}
		cordcpy(chainne,cor);
		if(1019-strlen(buf->chainne) > strlen(chainne)){
			strcat(buf->chainne,chainne);
			buf->chainne[strlen(buf->chainne)]='\0';
			Aff_Entete(fichi,4,strlen(buf->chainne));
		}else{
			cpt=1019-strlen(buf->chainne);j=0;
			strncat(buf->chainne,chainne,cpt);
			Ecriredir(fichi,nombre_de_bloc,buf);
			nombre_de_bloc++;
			alloc_bloc(fichi);
			Liredir(fichi,nombre_de_bloc,buf);
			j=strlen(chainne);
			strncpy(buf->chainne,&chainne[cpt],j-cpt);
			buf->chainne[j]='\0';
			Aff_Entete(fichi,4,strlen(buf->chainne));
		}	
	}
	if(strlen(buf->chainne)<1020){
			Ecriredir(fichi,nombre_de_bloc,buf);

	}
	Aff_Entete(fichi,2,1);
	Aff_Entete(fichi,4,strlen(buf->chainne));
	fermer(fichi);
	fclose(f );
	fclose(f1);
	fclose(f2);
	fclose(f3);
	printf("C'est bon.\n\nle resultat de l epuration : %d \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n",nombre_de_enrigistrement_effacer);}

/// le rolede ce module est d'inser un nouvel enrigistrement
void ins_new(){    
	char chainne[210];
	Buffer *buf=malloc(sizeof(Buffer));
	cord cor;
	char cordo[30];
	FILE *f=fopen(fichier_index,"r+");
	LnOVC *fichi=ouvrir(nomfich,'a');

	int j,cpt,nombre_de_bloc=Entete(fichi,1),trouv; 
	buf->chainne[0]='\0';
	Liredir(fichi,nombre_de_bloc,buf);
	while(1){
		trouv=0;
		printf("Matricule :");
		scanf("%s",&cor.matricule);
		if(111111<=atol(cor.matricule) && atol(cor.matricule)<=999999){
			sprintf(cordo,"/%d/%d\0",nombre_de_bloc,strlen(buf->chainne) );
			cordo[strlen(cordo)]='\0';
			check_place(f,cordo,cor.matricule,&trouv);
				if(trouv==0){
					get_new_enr(&cor);
					cordcpy(chainne,cor);
					if(1019-strlen(buf->chainne) > strlen(chainne)){
						strcat(buf->chainne,chainne);
						buf->chainne[strlen(buf->chainne)]='\0';
						Aff_Entete(fichi,4,strlen(buf->chainne));
					}else{
						cpt=1019-Entete(fichi,4);j=0;
						strncat(buf->chainne,chainne,cpt);
						Ecriredir(fichi,nombre_de_bloc,buf);
						nombre_de_bloc++;
						alloc_bloc(fichi);
						Liredir(fichi,nombre_de_bloc,buf);
						j=strlen(chainne);
						strncpy(buf->chainne,&chainne[cpt],j-cpt);
						buf->chainne[j]='\0';
						Aff_Entete(fichi,4,strlen(buf->chainne));
					}
					if(strlen(buf->chainne)<1020){
						Ecriredir(fichi,nombre_de_bloc,buf);
					}
					break;
				}else{
					printf("le matricule entre existe deja. \n voulez vous essayer a nouveau?\n    si 'oui' entre '1' ");
					scanf("%d",&trouv);
					if(trouv!=1){break;}
				}
		}else{printf("*********EROR:le matricule  doit etre entre 111111 et 999999\n");}
	}
	Aff_Entete(fichi,2,1);
	Aff_Entete(fichi,4,strlen(buf->chainne));
	fermer(fichi);
	fclose(f);
}

///le role de ce module est de modifier la region
void mod_rm(){
	char c,chainne[210],cordo[30];int mois,jour;
	Buffer *buf=malloc(sizeof(Buffer));
	buf->chainne[0]='\0';
	cord cor;long matricule;
	FILE *f=fopen(fichier_index,"r+"),*f1;
	LnOVC *fichi=ouvrir(nomfich,'a');
	printf("Entre le matricule : ");
	scanf("%d",&matricule);
	get_enr(&cor,matricule);
	supprimer(f,atol(cor.matricule));
	cor.regio_militaire[0]='\0';
	printf("Entre la nouvell region :\n");
	printf("   1RM-Blida\t2RM-Oran\t3RM-Bechar\n   4RM-Ouargla\t5RM-Constantine\t6RM-Tamanrasset\n");
	mois=1;
	scanf("%d",&jour);
	f1=fopen(region_mili,"r+");
	ecrire(f1,jour,matricule);
	fclose(f1);
	f1=fopen("region.txt","r");
	c=fgetc(f1);
	while(mois<jour){
		if(c=='\n'){mois++;}
		c=fgetc(f1);
	}
	mois=0;
	while(c!='\n' && c!=EOF){
		cor.regio_militaire[mois]=c;
		c=fgetc(f1);
		mois++;
	}
	cor.regio_militaire[mois]='\0';
	fclose(f1);
	int j,cpt,nombre_de_bloc=Entete(fichi,1),trouv;
	Liredir(fichi,nombre_de_bloc,buf);
	sprintf(cordo,"/%d/%d\0",nombre_de_bloc,strlen(buf->chainne) );
	cordo[strlen(cordo)]='\0';
	check_place(f,cordo,cor.matricule,&trouv);	
	cordcpy(chainne,cor);
	if(1019-strlen(buf->chainne) > strlen(chainne)){
		strcat(buf->chainne,chainne);
		buf->chainne[strlen(buf->chainne)]='\0';
		Aff_Entete(fichi,4,strlen(buf->chainne));
	}else{
		cpt=1019-Entete(fichi,4);j=0;
		strncat(buf->chainne,chainne,cpt);
		nombre_de_bloc++;
		buf->suivant=nombre_de_bloc;
		Ecriredir(fichi,nombre_de_bloc,buf);
		buf->chainne[0]='\0';
		while(cpt<strlen(chainne)){
			buf->chainne[j]=chainne[cpt];
			j++;cpt++;
		}
		buf->chainne[j]='\0';
		Aff_Entete(fichi,4,strlen(buf->chainne));	
	}
	Ecriredir(fichi,nombre_de_bloc,buf);
	buf->suivant=nombre_de_bloc;	
	Aff_Entete(fichi,1,nombre_de_bloc);
	Aff_Entete(fichi,2,1);
	Aff_Entete(fichi,3,nombre_de_bloc);
	Aff_Entete(fichi,4,strlen(buf->chainne));
	fermer(fichi);
	fclose(f);
}

///le rolle de ce module est de supprimmer un enrigistrement on donnent le matricule
void supp_enr(){
	long matricule;int trouv=0;
	char cor[30];
	FILE *f=fopen(fichier_index,"r+");
	printf("Entre le matricule : ");
	scanf("%d",&matricule);
	reche_place(cor,matricule,&trouv);
	if(trouv==1){
		supprimer(f,matricule);
	}else{printf("le matricule donnez n'existe pas\n");}
	fclose(f);
}

///le rolle de ce module est de supprimer tout les enrigistrements 
void supp_tt_force(){
	int choix;long matricule=1;
	FILE *f =fopen(force_arm,"r+");
	FILE *f1=fopen(fichier_index,"r+");
	printf("Quelle force voulez-vous supprimer : \n");
	printf("01.Armee de terre        \t02.Armee de l air\n03.Marine nationale        \t04.Defense aerienne du territoire\n05.Gendarmerie nationale  \t06.Garde republicaine \n07.Departement du renseignement et de la securite\t08.Sante militaire\n");	
	scanf("%d",&choix);
	fseek(f,6*(choix-1),SEEK_SET);
	while(matricule!=0){
		matricule=lire(f,1);
		supprimer(f1,matricule);
	}
	fclose(f1);
	fclose(f);
}

//le rolle de ce module est d'afichier
void afichier_region_age(){
	int i=0,choix,age,age_min,age_max;long matricule=1;
	char cordo[30];cord cor;
	FILE *f =fopen(region_mili,"r+");
	printf("Quelle region voulez-vous vous afichier :\n ");
	printf("   1RM-Blida\t2RM-Oran\t3RM-Bechar\n   4RM-Ouargla\t5RM-Constantine\t6RM-Tamanrasset\n");
	scanf("%d",&choix);
	printf("Entre l'intervale d'age :\n   l'age minimale : " );
	scanf("%d",&age_min);
	printf("   l'age maximale : ");
	scanf("%d",&age_max);
	fseek(f,6*(choix-1)+50,SEEK_SET);
				
	while(1){
		matricule=lire(f,0);	
		if(matricule==0){break;}
		else{
			get_enr(&cor,matricule);
			age=get_age(cor);
			if(age<=age_max && age>=age_min){
				//printf("\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",cor.matricule,cor.nom,cor.prenom,cor.date_de_naissance,cor.wilaya_de_naissance,cor.groupe_sanguin,cor.grade,cor.force_armee,cor.regio_militaire);
				printf("\nmatricule :           %s\n",cor.matricule);
				printf("nom :                 %s\n",cor.nom);
				printf("prenom :              %s\n",cor.prenom);
				printf("date de naissance :   %s\n",cor.date_de_naissance);
				printf("wilaya de naissance : %s\n",cor.wilaya_de_naissance);
				printf("groupe sanguin :      %s\n",cor.groupe_sanguin);
				printf("grade :               %s\n",cor.grade);
				printf("force armee :         %s\n",cor.force_armee);
				printf("region militaire :    %s\n",cor.regio_militaire);
				i++;
				if(i%2==0){
					printf("\n Si vous voulez quiter clic 'echap'\n");
					if(getch()==27){break;}
					system("cls");
				}
			}
		}
	}
	fclose(f);
}

//le rolle de ce module est d'afichier
void afichier_cat_grades(){
	int i=0,choix,age,age_min,age_max;long matricule=1;
	char cordo[30];cord cor;
	FILE *f =fopen(car_grade,"r+");
	while(choix<1 || choix>5){
		printf("Quelle categorie de grade voulez-vous vous afichier : \n");
		printf("   1-Officiers-generaux\n   2-officiers-superieurs\n   3-officiers\n   4-sous-officiers\n   5-hommes de troupes\n");
		scanf("%d",&choix);
		system("cls");
	}
	fseek(f,6*(choix-1)+50,SEEK_SET);
	while(1){
		matricule=lire(f,0);	
		if(matricule==0){break;}
		else{
			get_enr(&cor,matricule);
			if(atol(cor.matricule)!=0){
				//printf("\nmatricule : %s\nnom : %s\nprenom : %s\ndate de naissance : %s\nwilaya de naissance : %s\ngroupe sanguin : %s\ngrade : %s\nforce armee :%s\nregion militaire : %s\n",cor.matricule,cor.nom,cor.prenom,cor.date_de_naissance,cor.wilaya_de_naissance,cor.groupe_sanguin,cor.grade,cor.force_armee,cor.regio_militaire);
				printf("\nmatricule :           %s\n",cor.matricule);
				printf("nom :                 %s\n",cor.nom);
				printf("prenom :              %s\n",cor.prenom);
				printf("date de naissance :   %s\n",cor.date_de_naissance);
				printf("wilaya de naissance : %s\n",cor.wilaya_de_naissance);
				printf("groupe sanguin :      %s\n",cor.groupe_sanguin);
				printf("grade :               %s\n",cor.grade);
				printf("force armee :         %s\n",cor.force_armee);
				printf("region militaire :    %s\n",cor.regio_militaire);
				i++;
				if(i%2==0){
					printf("\n Si vous voulez quiter clic 'echap'\n");
					if(getch()==27){break;}
					system("cls");
				}
			}
		}
	}
	fclose(f);
}

void aff(){
	LnOVC *fichi=ouvrir(nomfich,'A');
	Buffer *buf=malloc(sizeof(Buffer));
	printf("L entete :\n  Nombre total de bloc alloués : %d\n  Numero du bloc representatnt la tete : %d \n",Entete(fichi,1),Entete(fichi,2));
    printf("  Numero du bloc representatnt la queue : %d\n  La postion libre dans le bloc de queue : %d\n",Entete(fichi,3),Entete(fichi,4));
    getch();
    system("cls");
    printf("Le nombre de Liredir : %d\nLe nombre de Ecriredir : %d\n",nombre_de_liredir,nombre_de_ecriredir );
    getch();
    system("cls");
    Liredir(fichi,1,buf);
    printf("%s\n",buf->chainne );
    getch();
    system("cls");
    Liredir(fichi,2,buf);
    printf("%s\n",buf->chainne );
    getch();
    system("cls");
    Liredir(fichi,2,buf);
    printf("%s\n",buf->chainne );
    system("cls");
    printf("le fichier des index\n");
    getch();
    system("fichier_index.txt");
    getch();	
    system("cls");
    printf("le fichier force armee\n");
    getch();
    system(force_arm);
    getch();
}

void menu(){
	 int i=0;
    printf("**************************************************************************************************************");
    printf("**************************************************************************************************************");
    printf("***************************         ***   ***  ******  ***   **   **   **          ***************************");
    printf("***************************         **** ****  **      ****  **   **   **          ***************************");
    printf("***************************         ** *** **  ******  ** ** **   **   **          ***************************");
    printf("***************************         **  *  **  **      **  ****   **   **          ***************************");
    printf("***************************         **     **  ******  **   ***   *******          ***************************");
    printf("**************************************************************************************************************");
    printf("**************************************************************************************************************");

    printf("\n");

    printf("**************************************************************************************************************");
    printf("**************************************************************************************************************");
    printf("**************************************************************************************************************");
    printf("**************************************************************************************************************");
    printf("***************************      1. creation et epuration                          ***************************");
    printf("***************************      2. insertion                                      ***************************");
    printf("***************************      3. modifier la region militaire                   ***************************");
    printf("***************************      4. supprimer un enregistrement                    ***************************");
    printf("***************************      5. supprimer tout une force                       ***************************");
    printf("***************************      6. afficher tous une region militaire             ***************************");
    printf("***************************      7. Consulter tous une categorie de grades         ***************************");
    printf("***************************      8. Afficher l en-tete et le N LireDir/EcrireDir   ***************************");
    printf("***************************      9. afichier les cordonners d un enregistrement    ***************************");
    printf("***************************     10. EXIT                                           ***************************");
    printf("**************************************************************************************************************");
    printf("**************************************************************************************************************");
    printf("**************************************************************************************************************");
    printf("**************************************************************************************************************");
}


void check_choix(){
    int choix;cord cor;
    while(1){
        system("cls");
        menu();
        while(1){
            printf("                                           ");
            scanf("%d",&choix);
            if(choix<11 && choix>0){break;}
            else{
                printf("EROR... vous devez choisir apartir du menu \n\n\n");
                system("pause");
                system("cls");
                menu();
            }
        }
           if   (choix == 1){
                system("cls");
                creation_epuration();
                system("pause");
        }else if(choix == 2){
            system("cls");
            printf("  ATENTION:  cette operation peux prendre des secondes\n");
            ins_new();
            system("pause");
            system("cls");
            printf("votre demende a ete bien effectue\n ");
        }else if(choix == 3){
            system("cls");
            printf("  ATENTION:  cette operation peux prendre des minutes \n");
            mod_rm();
            system("cls");
            printf("votre demende a ete bien effectue \n\n\n\n\n\n\n\n\n\n\n\n");
            system("pause");
        }else if(choix == 4){
            system("cls");
            printf("  ATENTION:  cette operation peux prendre des minutes \n");
            supp_enr();
            system("cls");
            printf("votre demende a ete bien effectue \n\n\n\n\n\n\n\n\n\n\n\n");
            system("pause");
        }else if(choix == 5){
            system("cls");
            printf("  ATENTION:  cette operation peux prendre des minutes \n");
            supp_tt_force();
            system("cls");
            printf("votre demende a ete bien effectue \n\n\n\n\n\n\n\n\n\n\n");
            system("pause");
        }else if(choix == 6){
            system("cls");
            printf("  ATENTION:  cette operation peux prendre des minutes \n");
            afichier_region_age();
            printf("votre demende a ete bien effectue \n\n\n\n\n\n\n\n\n\n\n");
            system("pause");
            system("cls");
        }else if(choix == 7){
            system("cls");
            afichier_cat_grades();
            system("cls");
        }else if(choix == 8){
            system("cls");
            aff();
            system("cls");
        }else if(choix == 9){
            system("cls");
            printf("Entre le matricule : ");
            scanf("%d",&choix);
            if(get_enr(&cor,choix)==1){
            	//printf("\nmatricule : %s\nnom : %s\nprenom : %s\ndate de naissance : %s\nwilaya de naissance : %s\ngroupe sanguin : %s\ngrade : %s\nforce armee :%s\nregion militaire : %s\n",cor.matricule,cor.nom,cor.prenom,cor.date_de_naissance,cor.wilaya_de_naissance,cor.groupe_sanguin,cor.grade,cor.force_armee,cor.regio_militaire);
       			printf("\nmatricule :           %s\n",cor.matricule);
				printf("nom :                 %s\n",cor.nom);
				printf("prenom :              %s\n",cor.prenom);
				printf("date de naissance :   %s\n",cor.date_de_naissance);
				printf("wilaya de naissance : %s\n",cor.wilaya_de_naissance);
				printf("groupe sanguin :      %s\n",cor.groupe_sanguin);
				printf("grade :               %s\n",cor.grade);
				printf("force armee :         %s\n",cor.force_armee);
				printf("region militaire :    %s\n",cor.regio_militaire);
       			//printf("\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",cor.matricule,cor.nom,cor.prenom,cor.date_de_naissance,cor.wilaya_de_naissance,cor.groupe_sanguin,cor.grade,cor.force_armee,cor.regio_militaire);
            }else{
            	printf("le matricule n existe pas.\n");
            }
            system("pause");
            system("cls");
            printf("votre demende a ete bien effectue \n\n\n\n\n\n\n\n\n\n\n");
            
        }else if(choix == 10){
        	break;
        }
    }
}

int main(){
    system ( " mode con cols=110 lines=28 " );
	setlocale(LC_ALL,"");
	srand(time(NULL));
	file_wilaya=fopen("wilaya.txt","r");
	file_grade =fopen("grade.txt" ,"r");
	file_region=fopen("region.txt","r");
	file_force =fopen("force.txt" ,"r");
	check_choix();
	fclose(file_wilaya);
	fclose(file_grade );
	fclose(file_region);
	fclose(file_force );
	return 0;
}
