/* A simple server in the internet domain using TCP
   The port number is passed as an argument 
   Esteban et Padrao EI2I3*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include <netdb.h>
#include <arpa/inet.h>

struct _client
{
         char ipAddress[40];
        int port;
        char name[40];
        int perdu;
        int gagne;
} tcpClients[4];

int nbClients;
int fsmServer;
int deck[13]={0,1,2,3,4,5,6,7,8,9,10,11,12};
int tableCartes[4][8];
char *nomcartes[]=
{"Sebastian Moran", "irene Adler", "inspector Lestrade",
  "inspector Gregson", "inspector Baynes", "inspector Bradstreet",
  "inspector Hopkins", "Sherlock Holmes", "John Watson", "Mycroft Holmes",
  "Mrs. Hudson", "Mary Morstan", "James Moriarty"};
int joueurCourant;
int JoueurQuest = 0;
int AccuseJoueur = 0;
int ObjDemande = 0;
int nbObjet = 0;
int joueurQuestionne = 0;
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void melangerDeck()
{
        int i;
        int index1,index2,tmp;

        for (i=0;i<1000;i++)
        {
                index1=rand()%13;
                index2=rand()%13;

                tmp=deck[index1];
                deck[index1]=deck[index2];
                deck[index2]=tmp;
        }
}

void createTable()
{
	// Le joueur 0 possede les cartes d'indice 0,1,2
	// Le joueur 1 possede les cartes d'indice 3,4,5 
	// Le joueur 2 possede les cartes d'indice 6,7,8 
	// Le joueur 3 possede les cartes d'indice 9,10,11 
	// Le coupable est la carte d'indice 12
	int i,j,c;

	for (i=0;i<4;i++)
		for (j=0;j<8;j++)
			tableCartes[i][j]=0;

	for (i=0;i<4;i++)
	{
		for (j=0;j<3;j++)
		{
			c=deck[i*3+j];
			switch (c)
			{
				case 0: // Sebastian Moran
					tableCartes[i][7]++;
					tableCartes[i][2]++;
					break;
				case 1: // Irene Adler
					tableCartes[i][7]++;
					tableCartes[i][1]++;
					tableCartes[i][5]++;
					break;
				case 2: // Inspector Lestrade
					tableCartes[i][3]++;
					tableCartes[i][6]++;
					tableCartes[i][4]++;
					break;
				case 3: // Inspector Gregson 
					tableCartes[i][3]++;
					tableCartes[i][2]++;
					tableCartes[i][4]++;
					break;
				case 4: // Inspector Baynes 
					tableCartes[i][3]++;
					tableCartes[i][1]++;
					break;
				case 5: // Inspector Bradstreet 
					tableCartes[i][3]++;
					tableCartes[i][2]++;
					break;
				case 6: // Inspector Hopkins 
					tableCartes[i][3]++;
					tableCartes[i][0]++;
					tableCartes[i][6]++;
					break;
				case 7: // Sherlock Holmes 
					tableCartes[i][0]++;
					tableCartes[i][1]++;
					tableCartes[i][2]++;
					break;
				case 8: // John Watson 
					tableCartes[i][0]++;
					tableCartes[i][6]++;
					tableCartes[i][2]++;
					break;
				case 9: // Mycroft Holmes 
					tableCartes[i][0]++;
					tableCartes[i][1]++;
					tableCartes[i][4]++;
					break;
				case 10: // Mrs. Hudson 
					tableCartes[i][0]++;
					tableCartes[i][5]++;
					break;
				case 11: // Mary Morstan 
					tableCartes[i][4]++;
					tableCartes[i][5]++;
					break;
				case 12: // James Moriarty 
					tableCartes[i][7]++;
					tableCartes[i][1]++;
					break;
			}
		}
	}
} 

void printDeck()
{
        int i,j;

        for (i=0;i<13;i++)
                printf("%d %s\n",deck[i],nomcartes[deck[i]]);

	for (i=0;i<4;i++)
	{
		for (j=0;j<8;j++)
			printf("%2.2d ",tableCartes[i][j]);
		puts("");
	}
}

void printClients()
{
        int i;

        for (i=0;i<nbClients;i++)
                printf("%d: %s %5.5d %s\n",i,tcpClients[i].ipAddress,
                        tcpClients[i].port,
                        tcpClients[i].name);
}

int findClientByName(char *name)
{
        int i;

        for (i=0;i<nbClients;i++)
                if (strcmp(tcpClients[i].name,name)==0)
                        return i;
        return -1;
}

void sendMessageToClient(char *clientip,int clientport,char *mess)
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server = gethostbyname(clientip);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(clientport);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        {
                printf("ERROR connecting\n");
                exit(1);
        }

        sprintf(buffer,"%s\n",mess);
        n = write(sockfd,buffer,strlen(buffer));

    close(sockfd);
}

void broadcastMessage(char *mess)
{
        int i;

        for (i=0;i<nbClients;i++)
                sendMessageToClient(tcpClients[i].ipAddress,
                        tcpClients[i].port,
                        mess);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n, templose = 0;
	 int i, y = 0, cp = 0;


        char com;
        char clientIpAddress[256], clientName[256];
        int clientPort;
        int id;
        char reply[256];


     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);

	printDeck();
	melangerDeck();
	createTable();
	printDeck();
	joueurCourant=0;

	for (i=0;i<4;i++)
	{
        	strcpy(tcpClients[i].ipAddress,"localhost");
        	tcpClients[i].port=-1;
        	strcpy(tcpClients[i].name,"-");
	}

     while (1)
     {    
     	newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
     	if (newsockfd < 0) 
          	error("ERROR on accept");

     	bzero(buffer,256);
     	n = read(newsockfd,buffer,255);
     	if (n < 0) 
		error("ERROR reading from socket");

        printf("Received packet from %s:%d\nData: [%s]\n\n",
                inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buffer);

        if (fsmServer==0)
        {
        	switch (buffer[0])
        	{
                	case 'C':
                        	sscanf(buffer,"%c %s %d %s", &com, clientIpAddress, &clientPort, clientName);
                        	printf("COM=%c ipAddress=%s port=%d name=%s\n",com, clientIpAddress, clientPort, clientName);

                        	// fsmServer==0 alors j'attends les connexions de tous les joueurs
                                strcpy(tcpClients[nbClients].ipAddress,clientIpAddress);
                                tcpClients[nbClients].port=clientPort;
                                strcpy(tcpClients[nbClients].name,clientName);
                                nbClients++;

                                printClients();

				// rechercher l'id du joueur qui vient de se connecter

                                id=findClientByName(clientName);
                                printf("id=%d\n",id);

				// lui envoyer un message personnel pour lui communiquer son id

                                sprintf(reply,"I %d",id);
                                sendMessageToClient(tcpClients[id].ipAddress,
                                       tcpClients[id].port,
                                       reply);

				// Envoyer un message broadcast pour communiquer a tout le monde la liste des joueurs actuellement
				// connectes

                                sprintf(reply,"L %s %s %s %s", tcpClients[0].name, tcpClients[1].name, tcpClients[2].name, tcpClients[3].name);
                                broadcastMessage(reply);

				// Si le nombre de joueurs atteint 4, alors on peut lancer le jeu

                                if (nbClients==4)
				{
					// On envoie ses cartes au joueur 0, ainsi que la ligne qui lui correspond dans tableCartes
					
					sprintf(reply,"D %d %d %d",deck[0], deck[1], deck[2]);
                    sendMessageToClient(tcpClients[0].ipAddress, tcpClients[0].port, reply);
                    

                    for(y = 0; y <8; y++)
					{
					sprintf(reply,"V 0 %d %d ", y, tableCartes[0][y]);
                    sendMessageToClient(tcpClients[0].ipAddress, tcpClients[0].port, reply);    
					}                
                                        

					// On envoie ses cartes au joueur 1, ainsi que la ligne qui lui correspond dans tableCartes
					
					sprintf(reply,"D %d %d %d",deck[3], deck[4], deck[5]);
                    sendMessageToClient(tcpClients[1].ipAddress, tcpClients[1].port, reply);
                    
                    for(y = 0; y <8; y++)
					{
					sprintf(reply,"V 1 %d %d ", y, tableCartes[1][y]);
                    sendMessageToClient(tcpClients[1].ipAddress, tcpClients[1].port, reply);    
					}     

					// On envoie ses cartes au joueur 2, ainsi que la ligne qui lui correspond dans tableCartes
					
					sprintf(reply,"D %d %d %d",deck[6], deck[7], deck[8]);
                    sendMessageToClient(tcpClients[2].ipAddress, tcpClients[2].port, reply);
                    
                    for(y = 0; y <8; y++)
					{
					sprintf(reply,"V 2 %d %d ", y, tableCartes[2][y]);
                    sendMessageToClient(tcpClients[2].ipAddress, tcpClients[2].port, reply);    
					}   

					// On envoie ses cartes au joueur 3, ainsi que la ligne qui lui correspond dans tableCartes
					
					sprintf(reply,"D %d %d %d",deck[9], deck[10], deck[11]);
                    sendMessageToClient(tcpClients[3].ipAddress, tcpClients[3].port, reply);
                    
                    for(y = 0; y <8; y++)
					{
					sprintf(reply,"V 3 %d %d ", y, tableCartes[3][y]);
                    sendMessageToClient(tcpClients[3].ipAddress, tcpClients[3].port, reply);    
					}   

					// On envoie enfin un message a tout le monde pour definir qui est le joueur courant=0

					for(y = 0; y <3; y++)
					{
					sprintf(reply,"M %d ",joueurCourant);
					sendMessageToClient(tcpClients[y].ipAddress, tcpClients[y].port, reply);
					}
						
                                        fsmServer=1;
				}
				break;
                }
	}
	else if (fsmServer==1)
	{
		switch (buffer[0])
		{
                	case 'G':
				// RAJOUTER DU CODE ICI
				sscanf(buffer,"%c %d %d", &com, &JoueurQuest, &AccuseJoueur);
                printf("COM=%c JoueurQuest=%d AccuseJoueur=%d\n",com, JoueurQuest, AccuseJoueur);	
                if(AccuseJoueur == deck[12])
                {
					tcpClients[JoueurQuest].gagne = 1;
					printf("Félicitation, %s a gagné !\n\n", tcpClients[JoueurQuest].name);
					sprintf(reply,"W %s", tcpClients[JoueurQuest].name);
					broadcastMessage(reply);
					close(newsockfd);   			
					close(sockfd);
					return 0;

				}
				else
				{
					tcpClients[JoueurQuest].perdu = 1;
					templose = JoueurQuest;
					printf("Mince alors ! %s a perdu !\n\n", tcpClients[JoueurQuest].name);
					sprintf(reply,"P %s", tcpClients[JoueurQuest].name);
					broadcastMessage(reply);
					
					if((tcpClients[0].perdu == 1)&&(tcpClients[1].perdu == 1)&&(tcpClients[2].perdu == 1)&&(tcpClients[3].perdu == 1))//si tous les joueurs on perdu on quitte
					{
						sprintf(reply,"F");
						broadcastMessage(reply);
						close(newsockfd);   			
						close(sockfd);
						return 0;
					}
					
				}								
				
				break;
                	case 'O':
				// RAJOUTER DU CODE ICI
				
				sscanf(buffer,"%c %d %d", &com, &JoueurQuest, &ObjDemande);
                printf("COM=%c JoueurQuest=%d ObjDemande=%d\n",com, JoueurQuest, ObjDemande);
					templose = JoueurQuest;
				for(y = 0; y < 4; y++)
				{
					for(cp = 0; cp < 4; cp++)
					{
						if(cp == JoueurQuest)
						{

						}
						else
						{
							if(tableCartes[cp][ObjDemande] == 0)
							{
								sprintf(reply,"V %d %d %d ", cp, ObjDemande, tableCartes[cp][ObjDemande]);
								sendMessageToClient(tcpClients[y].ipAddress, tcpClients[y].port, reply);
							}
							else
							{    
								sprintf(reply,"V %d %d 100 ", cp, ObjDemande);
								sendMessageToClient(tcpClients[y].ipAddress, tcpClients[y].port, reply);
							}
						}
					}
				}
				 
				  					
				
				break;
			case 'S':
				// RAJOUTER DU CODE ICI
				
				sscanf(buffer,"%c %d %d %d", &com, &JoueurQuest, &joueurQuestionne, &ObjDemande);
                printf("COM=%c JoueurQuest=%d joueurQuestionne = %d ObjDemande=%d\n",com, JoueurQuest, joueurQuestionne, ObjDemande);
					templose = JoueurQuest;
				for(y = 0; y < 4; y++)
				{
					
						if(joueurQuestionne == JoueurQuest)
						{

						}
						else
						{

								sprintf(reply,"V %d %d %d ", joueurQuestionne, ObjDemande, tableCartes[joueurQuestionne][ObjDemande]);
								sendMessageToClient(tcpClients[y].ipAddress, tcpClients[y].port, reply);

						}
					
				}

				sprintf(reply,"V %d %d %d ", JoueurQuest, ObjDemande, tableCartes[JoueurQuest][ObjDemande]);
				sendMessageToClient(tcpClients[JoueurQuest].ipAddress, tcpClients[JoueurQuest].port, reply);  
				
				break;
                	default:
                        	break;
		}

				if(JoueurQuest == 3)
				{
					joueurCourant = 0;
				}
				else
				{
					joueurCourant = JoueurQuest + 1;
				}

				while (tcpClients[joueurCourant].perdu == 1)
				{
					if(joueurCourant == 3)
					{
						joueurCourant =0;
					}
					else
					{
						joueurCourant++;
					}
				}
				
						
					
				sprintf(reply,"M %d ",joueurCourant);
				broadcastMessage(reply);
					
     }
     	close(newsockfd);
     			
					
     }
     close(sockfd);
     return 0; 
}
