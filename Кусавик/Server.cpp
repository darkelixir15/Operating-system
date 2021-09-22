#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <time.h>

using namespace std;


void printmas(int* mas, int s)
{
    for (int i = 0; i < s; i++)
    {
        cout <<  mas[i] << " ";
    } 
    cout << endl;
}

void shake(int* c)
{
    int ind; int temp;
    srand(time(NULL));
    for(int i = 0; i < 36; i++)
    {
        ind = rand()%(36-i) + i;
        temp = c[i];
        c[i] = c[ind];
        c[ind] = temp;
    }
}

bool copm(int kart, int* ruki, int s)
{
    for(int i = 0; i < s; i++)
    {
        if( ruki[i] == kart )
        {
            return 0;
        }
    }
    return 1;
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

    bool bitcheck(int k1, int k2, int kosr) //(защита, нападение)
    {
        if((k1/10 == kosr/10) && (k2/10 != kosr/10))
        { return 1;}
        else if((k1/10 != kosr/10) && (k2/10 == kosr/10))
        {return 0;}
        else if(k1/10 != k2/10)
        {return 0;}
        else
        {
            if(k1%10 > k2%10)
            {return 1;}
            else
            {return 0;}
        }

    }

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "Russian");

    int port = atoi(argv[1]);
    struct sockaddr_in server_addr;

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = PF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("socket() error");
        exit(-1);
    }
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        server_addr.sin_port = htons(port+1);;
        if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        {
            perror("bind() error");
            exit(-1);
        }
    }
    listen(sock, 2);
    cout << "Listening on port: " << ntohs(server_addr.sin_port) << endl;
    int cards[36] 
    {
     10,11,12,13,14,15,16,17,18,
     20,21,22,23,24,25,26,27,28,
     30,31,32,33,34,35,36,37,38,
     40,41,42,43,44,45,46,47,48
    };
    shake(cards);

cout << "Ожидание игроков..." << endl;
//---------ОЖИДАНИЕ ИГРОКОВ-------------
    int newSock1 = accept(sock, NULL, NULL);
    int gn1 = 1;
    write(newSock1, &gn1, 4);//отправить первому игроку его номер
    int newSock2 = accept(sock, NULL, NULL);
    int gn2 = 2;
    write(newSock2, &gn2, 4); //отправка второму игроку его номер
    char con = 0;
    write(newSock1, &con, 1);//говорим игрокам, что все вошли
    write(newSock2, &con, 1); 
    char rdy1,rdy2;
    read(newSock1, &rdy1, 4);//ожидаем готовность игроков
    read(newSock2, &rdy2, 4);
    if(rdy1 == '+' && rdy2 == '+')
    {
        write(newSock1, &con, 1);//говорим всем, что все готовы
        write(newSock2, &con, 1);
    }
//--------------ИНИЦИАЛИЗАЦИИ-----------
    shake(cards);
    int jj = 0; int gamer1size = 6; int gamer2size = 6;

    int* cards_for_gamer1 = new int[gamer1size];
    for (int k = 0; k<6; k++)
    {
        cards_for_gamer1[k] = cards[jj];
        jj++;
    }

    write(newSock1, cards_for_gamer1, 24);//отправляем первому его карты
    int* cards_for_gamer2 = new int[gamer2size];
    for (int k = 0; k<6; k++)
    {
        cards_for_gamer2[k] = cards[jj];
        jj++;
    }
    write(newSock2, cards_for_gamer2, 24);//отправляем второму его карты
    int kosr = cards[35];

    write(newSock1, &kosr, 4);//отправляем обоим козырь
    write(newSock2, &kosr, 4);
//---------ИГРА---------------------------
    int karta1;int karta2;int perehod; int bitnebit;int razmerost;
    int napad = 1;// 1- нападение, 0 - защита
    int i = 0; int raund = 0;

while(1)
{ 
    //cout << "napad " << napad << endl;
    //cout << "raund " << raund << endl;
    //--------------первый игрок------------
    if((i%2) == 0)
    { 
        write(newSock1, &gn1, 4);
        if(raund == 0)
        {
            write(newSock2, &gn1, 4);
        }
        write(newSock1, &napad, 4);
       
        if(napad == 1)//-------нападение первого игрока
        {
            //cout << "говорим что первый нападает" << endl;
            while(1)
            {
                read(newSock1, &karta1, 4);
                if(copm(karta1, cards_for_gamer1, gamer1size) == 1 )
                {
                    perehod = 0;
                    write(newSock1, &perehod, 4);//сообщаем, что ход неудачный 
                    continue;
                }
                else
                {
                    perehod = 1; 
                    write(newSock1, &perehod, 4);//сообщаем, что ход успешный 
                    razmerost = 36 - jj ;
                    write(newSock1, &razmerost, 4);//передаем остаток в колоде
                    //изменение в картах на руках
                    if(gamer1size==1 && razmerost==0)
                    {
                        napad = 3;
                        write(newSock2, &napad, 4);
                    }
                    else if(gamer1size < 7 && razmerost > 1)
                    {
                        write(newSock1, &cards[jj], 4); 
                        rukiupdate(cards_for_gamer1, karta1 , cards[jj], gamer1size);
                        jj++;
                    }
                    else if(gamer1size < 7 && razmerost == 1)
                    {
                        write(newSock1, &kosr, 4); jj++;
                    }
                    else if(gamer1size < 7 && razmerost == 0)
                    {
                        cards_for_gamer1 = resizek(cards_for_gamer1, gamer1size, gamer1size-1);
                        gamer1size--;
                    }
                    napad = 0;
                    i++;
                    raund = 0;
                    write(newSock2, &napad, 4);
                    break;
                }
            }
            continue;
        }
        if(napad == 0)//защита первого игрока
        { 
            //cout << "Защита первого" << endl;
            while(1)
            {
                    write(newSock1, &karta1, 4);
                    read(newSock1, &bitnebit, 4);
                    if (bitnebit)
                    {
                        cards_for_gamer1 = resizek(cards_for_gamer1, gamer1size, gamer1size+1, karta1);
                        gamer1size++;
                        napad = 1;
                        write(newSock1, &razmerost, 4);
                        write(newSock2, &napad, 4);
                        i++;
                        break;
                    }
                    else if(!bitnebit)
                    {
                        read(newSock1, &karta2, 4);
                        if(copm(karta2, cards_for_gamer1, gamer1size))
                        {
                            perehod = 0;
                            write(newSock1, &perehod, 4);
                            continue;
                        }
                        else if(!copm(karta2, cards_for_gamer1, gamer1size))
                        {
                            if (bitcheck(karta2, karta1, kosr))
                            {
                                perehod = 1;
                                write(newSock1, &perehod, 4);
                                //-------------------------
                                razmerost = 36 - jj;
                                write(newSock1, &razmerost, 4);//передаем остаток в колоде
                                //изменение в картах на руках
                                if(gamer1size==1 && razmerost==0)
                                {
                                    napad = 3;
                                    write(newSock2, &napad, 4);
                                }
                                if(gamer1size < 7 && razmerost > 1)
                                {
                                    write(newSock1, &cards[jj], 4); 
                                    rukiupdate(cards_for_gamer1, karta2 , cards[jj], gamer1size);
                                    jj++;
                                }
                                if(gamer1size < 7 && razmerost == 1)
                                {
                                    write(newSock1, &kosr, 4); jj++;
                                }
                                if(gamer1size < 7 && razmerost == 0)
                                {
                                    cards_for_gamer1 = resizek(cards_for_gamer1, gamer1size, gamer1size-1);
                                    gamer1size--;
                                }
                                //-------------------------
                                napad = 1; raund= 1;
                                write(newSock1, &napad, 4);
                                break;
                            }else
                            {
                                perehod = 0;
                                write(newSock1, &perehod, 4);
                                continue;
                            }
                        }
                    }
                    else
                    {
                        continue;
                    }       
            }
            continue; 
        }   
    }
    else
    { 
       // cout << "второй" << endl;
        if(raund == 0)
        {
            write(newSock1, &gn2, 4);
        }
        write(newSock2, &gn2, 4);
        write(newSock2, &napad, 4);
        if(napad == 1)   //нападение второго игрока
        { 
            //cout <<"нападение второго" << endl;
            while(1)
            {
                read(newSock2, &karta1, 4);
                if(copm(karta1, cards_for_gamer2, gamer2size))
                {
                    perehod = 0;
                    write(newSock2, &perehod, 4);//сообщаем, что ход неудачный 
                    continue;
                }
                else if(!copm(karta1, cards_for_gamer2, gamer2size))
                {
                    perehod = 1; 
                    write(newSock2, &perehod, 4);//сообщаем, что ход успешный 
                    razmerost = 36 - jj;
                    write(newSock2, &razmerost, 4);//передаем остаток в колоде
                    if(gamer2size==0 && razmerost==0)
                    {
                        napad = 3;
                        write(newSock1, &napad, 4);
                    }
                    if(gamer2size < 7 && razmerost > 1)
                    {
                        write(newSock2, &cards[jj], 4); 
                        rukiupdate(cards_for_gamer2, karta1 , cards[jj], gamer2size);
                        jj++;
                    }
                    if(gamer2size < 7 && razmerost == 1)
                    {
                        write(newSock2, &kosr, 4); jj++;
                    }
                    if(gamer2size < 7 && razmerost == 0)
                    {
                        cards_for_gamer2 = resizek(cards_for_gamer2, gamer2size, gamer2size-1);
                        gamer2size--;
                    }
                    napad = 0;
                    i++;
                    raund = 0;
                    write(newSock1, &napad, 4);
                    break;
                }
            }    
            continue;
        } 
        if(napad == 0) //защита второго игрока
        { 
            //cout << "Защита втрого"<< endl;
            while(1)
            { 
                write(newSock2, &karta1, 4);//говорим другому чем на него походили
                read(newSock2, &bitnebit, 4);
             
                if (bitnebit == 1)
                {
                    cards_for_gamer2 = resizek(cards_for_gamer2, gamer2size, gamer2size+1, karta1);
                    gamer2size++;
                    napad = 1;
                    write(newSock2, &razmerost, 4);
                    write(newSock1, &napad, 4);
                    i++;
                    break;
                }else if(!bitnebit)
                {
                    read(newSock2, &karta2, 4);
                    if(copm(karta2, cards_for_gamer2, gamer2size))
                    {
                        perehod = 0;
                        write(newSock2, &perehod, 4);
                        continue;
                    }
                    else if(!copm(karta2, cards_for_gamer2, gamer2size))
                    {
                        if (bitcheck(karta2, karta1, kosr))
                        {
                            perehod = 1;
                            write(newSock2, &perehod, 4);
                                         //-------------------------
                            razmerost = 36 - jj;
                            write(newSock2, &razmerost, 4);//передаем остаток в колоде
                                    //изменение в картах на руках
                            if(gamer2size==1 && razmerost==0)
                            {
                                napad = 3;
                                write(newSock2, &napad, 4);
                            }
                            if(gamer2size < 7 && razmerost > 1)
                            {
                                write(newSock2, &cards[jj], 4); 
                                rukiupdate(cards_for_gamer2, karta2 , cards[jj], gamer2size);
                                jj++;
                            }
                            if(gamer2size < 7 && razmerost == 1)
                            {
                                write(newSock2, &kosr, 4); jj++;
                                rukiupdate(cards_for_gamer2, karta1 , kosr, gamer2size);
                            }
                            if(gamer2size < 7 && razmerost == 0)
                            {
                                cards_for_gamer2 = resizek(cards_for_gamer2, gamer2size, gamer2size-1);
                                gamer2size--;
                            }
                            napad = 1;
                            raund++;
                            break;
                        }else
                        {
                            perehod = 0;
                            write(newSock2, &perehod, 4);
                            continue;
                        }
                    }
                }
            }
            continue;   
        }
    }
}
return 0;
}
