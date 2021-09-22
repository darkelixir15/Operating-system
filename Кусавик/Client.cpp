#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

using namespace std;

void rukiupdate(int* ruki, int karta, int zamena, int size)
{
	for(int i = 0; i < size; i++)
	{
		 if(ruki[i] == karta)
		 {
		 	ruki[i] = zamena;
		 	break;
		 }
	}
}

void interpret(int k)
{
	if(k%10 == 0)
        {cout << "Шесть ";}
        else if(k%10 == 1)
        {cout << "Семь "; }
         else if(k%10 == 2)
        {cout << "Восемь ";}
        else if(k%10 == 3)
        {cout << "Девять ";}
        else if(k%10 == 4)
        {cout << "Десять ";}
        else if(k%10 == 5)
        {cout << "Валет "; }
        else if(k%10 == 6)
        {cout << "Дама ";}
        else if(k%10 == 7)
        {cout << "Король ";}
        else if(k%10 == 8)
        {cout << "Туз ";}

        if(k/10 == 1)
        {cout << "Черви ";}
         else if(k/10 == 2)
        {cout << "Бубен ";}
         else if(k/10 == 3)
        {cout << "Пики ";}
        else if(k/10 == 4)
        {cout << "Крести ";}
        cout << endl;
}

void printmas(int* mas, int size)
{
    for (int i = 0; i < size; i++)
    {	
    	cout << "-----";
        interpret(mas[i]);
    }
    cout << endl;
}

int trans(char* line, char* line2)
{	
 	char transed[2];
 			
 			if(!strncmp(line, "Шесть", 5))
 			{transed[1] = '0';}
 			else if(!strncmp(line, "Семь", 4))
 			{transed[1] = '1';}
 			else if(!strncmp(line, "Восемь", 6))
 			{transed[1] = '2';}
 			else if(!strncmp(line, "Девять", 6))
 			{transed[1] = '3';}
 			else if(!strncmp(line, "Десять", 6))
 			{transed[1] = '4';}
 			else if(!strncmp(line, "Валет", 5))
 			{transed[1] = '5';}
 			else if(!strncmp(line, "Дама", 4))
 			{transed[1] = '6';}
 			else if(!strncmp(line, "Король", 6))
 			{transed[1] = '7';}
 			else if(!strncmp(line, "Туз", 3))
 			{transed[1] = '8';}

 			if(!strncmp(line2, "Черви", 5))
 			{transed[0] = '1';}
 			else if(!strncmp(line2, "Бубен", 5))
 			{transed[0] = '2';}
 			else if(!strncmp(line2, "Пики", 4))
 			{transed[0] = '3';}
 			else if(!strncmp(line2, "Крести", 6))
 			{transed[0] = '4';}

 			return atoi(transed);
 	}

int ipget(char* arg, char* ip)
{
    int iplen;
    int i = 0;
    while(arg)
    {
        if(arg[i] == ':')
        {
            iplen = i;
            break;
        }
        i++;
    }
    i = 0;
    for (char* l = ip; l < ip + iplen ; l++)
    {
        *l = *(arg + i);
        i++;
    }
    *(ip + i) = '\0';
    return iplen;
}

int* resizek(int *mas, int size, int new_size, int karta = 0)
{
	int*mm = new int[new_size]; int index = 0;
	
	if(size > new_size)
	{
		for(int i = 0; i < size; i++)
	    {
	    	if(mas[index] == karta)
	    	{
	    		break;
	    	}
	    	index++;
	    }
		int tmp = mas[size-1];
		mas[size-1] = mas[index];
		mas[index] = tmp;
	}

	for(int p = 0; p < new_size; p++)
	{
		if (p==size)
	    {
	      mm[p]=karta;
	    }
	    else
	    {	
	    	mm[p]=mas[p];
	    }
	}
	delete []mas;
	return mm;
		
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	 if(argc != 2 )
    {cout << "Error: program needs socket." << endl;}
    int ipl; char ip[15];
    ipl = ipget(argv[1], ip);
    char* prt = new char[strlen(argv[1]) - ipl + 1];
    prt = argv[1] + ipl+1;

    struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = PF_INET;
    server_addr.sin_port = htons(atoi(prt));
    inet_aton(ip, &server_addr.sin_addr);

   int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket() error");
        exit(-1);
    }

   if(connect(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect() error");
        close(sockfd);
        exit(-1);
    }
//-----СБОР ИГРОКОВ--------
    int n;
    read(sockfd, &n, 4);//номер игрока
    cout << "Вы игрок №" << n << endl << endl;
    char con = 1;
    read(sockfd, &con, 1);//ожидание подключения второго игрока
    if(!con)
    {
    	cout << "Игроки в сборе" << endl;
    }
 	cout << "Введите +, если готовы." << endl;
	char rdy;
	while(1)
	{
	    cin >> rdy;
	    if (rdy == '+')
	    {
	    	break;
	    }
		else
		{
			cout << "Это не плюс." << endl;
			continue;
		}
	}
	write(sockfd, &rdy, 1);//отправка подтверждения о готовности
	read(sockfd, &con, 1);//сигнал о готовности обоих игроков
	if(!con)
	{cout << "Игроки готовы, игра началась." << endl;}
//--------ИНИЦИАЛИЗАЦИИ----------------
	int rukisize = 6;
	int* ruki = new int[rukisize];
	int kosr;
 	read(sockfd, ruki, 24);//прием выданных начальных карт
 	read(sockfd, &kosr, 4);//прием инфы о козыре
 	cout << "Козырь: " <<endl;
 	interpret(kosr);
 	cout << endl;


 	int hod;char line[32]; int karta;
 	char line2[32]; int perehod;
 	int napad = 1;int karta2; int bitnebit;
 	 int vzialkartu; int razmerost = 24;

 //-----------ИГРА--------------
 while(1)
 {
 	read(sockfd, &hod, 4);//инфа о том, чей сейчас ход
 	cout << "Ваши Карты" << endl;
 	printmas(ruki, rukisize);
 	//cout << "hod = "<< hod << endl;
 	if (hod == n)
 	{
 		read(sockfd, &napad, 4);
 			if(napad == 1)//нападение
	 		{	
	 			while(1)
		 		{
		 			cout << "--------------Вы нападаете------(в общей колоде " << razmerost<< " карт)"<< endl;
			 		cout << "Козырь: " ;
			 		interpret(kosr);
			 		cout << "Выберите карту." << endl;
			 		cin >> line >> line2;
			 		karta = trans(line, line2);
			 		write(sockfd, &karta, 4);//отправка выбранной карты
			 		read(sockfd, &perehod, 4);//инфа о том, есть ли выбраная карта у игрока
			 		if (perehod)
			 		{ 
			 			read(sockfd, &razmerost, 4);
			 			if(rukisize == 1 && razmerost == 0)
			 			{
			 				cout << "Поздравляем, вы победили!" << endl;
			 				close(sockfd);
			 				exit(0);
			 			}
			 			else if(rukisize < 7 && razmerost!=0)
			 			{
			 				read(sockfd, &vzialkartu, 4);
			 				rukiupdate(ruki, karta , vzialkartu,rukisize);
			 			}
			 			else if((rukisize < 7 && razmerost == 0) || rukisize > 6)
			 			{ 
			 				ruki = resizek(ruki, rukisize, rukisize-1);
			 				rukisize--;
			 				
			 			}
			 			cout << "Переход хода." << endl;
			 			break;
			 		}
			 		else
			 		{
			 			cout << "У вас нет такой карты!" << endl;
			 			continue;
			 		}
			 	}
			 	continue;
			}
	 		if(napad == 0)//защита
	 		{	
	 			
	 			while(1)
	 			{	
		 			cout << "------------Защищайтесь------(в общей колоде " << razmerost<< " карт)"<< endl;
		 			cout << "Козырь: ";
		 			interpret(kosr);
		 			cout << "На вас походили картой  ";
		 			read(sockfd, &karta, 4);
	 				interpret(karta); cout << endl;
			 		cout << "ВВЕДИТЕ 1, ЕСЛИ РЕШИЛИ ПРИНЯТЬ КАРТУ ИЛИ 0, ЧТОБЫ ОТБИВАТЬСЯ" << endl;
			 		cin >> bitnebit;
			 		write(sockfd, &bitnebit,4);//говорим принимаем или бьемся
			 		if(bitnebit == 1)
			 		{
			 			ruki = resizek(ruki, rukisize, rukisize+1, karta);
			 			rukisize++;
			 			read(sockfd, &razmerost, 4);
			 			break;
			 		}
			 		else if(!bitnebit)
					{
				 		cout << "Выберите карту" << endl;
				 		cin >> line >> line2;
				 		karta2 = trans(line, line2);
				 		write(sockfd, &karta2, 4);//отправка выбранной карты
				 		read(sockfd, &perehod, 4);//инфа о том, есть ли выбраная карта у игрока
				 		if (perehod)
				 		{
				 			read(sockfd, &razmerost, 4);
				 			if(rukisize == 1 && razmerost == 0)
				 			{
				 				cout << "Поздравляем, вы победили!" << endl;
				 				close(sockfd);
				 				exit(0);
				 			}
				 			if(rukisize < 7 && razmerost!=0)
				 			{
				 				read(sockfd, &vzialkartu, 4);
				 				rukiupdate(ruki, karta2 , vzialkartu, rukisize);
				 			}

				 			if((rukisize < 7 && razmerost == 0) || rukisize > 6)
				 			{
				 				ruki = resizek(ruki, rukisize, rukisize-1);
				 				rukisize--;
				 			}
				 			cout << "------Бито!------" << endl;
				 			break;
				 		}
				 		else
				 		{
				 			cout << "У вас нет такой карты!" << endl;
				 			continue;
				 		}
				 	}
	 			}
	 			continue;
 			}
 	}
	else
 	{
 		cout << "Ход оппонента..." << endl;
 		read(sockfd, &napad, 4);
 		if(napad == 3)
 		{
 			cout << "Игра окончена. Вы дурак." << endl;
 			close(sockfd);
 			exit(0);
 		}
 	} 
}
   return 0;
}
