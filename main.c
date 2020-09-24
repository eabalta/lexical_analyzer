#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define MAX_ID 20
#define MAX_INT 10

//prototypes
int checkKeyword(char* control, int count,FILE *lex);
int checkInteger(char* control, int count,FILE *lex);
void writeIdentifier(char* control, int count,FILE *lex);


int main()
{
    // Dosya Islemleri
    FILE *psi,*lex;
    if((psi = fopen("code.psi","r"))==NULL)
    {
        printf("%s","File not found.");
        return -1;
    }
    lex = fopen("code.lex","w");

    // Okunan kodda s�n�rlama yapan t�m de�erler delimiters i�inde topland�.
    char delimiters[]       = {'+','-','*','/',':','=','(',')','[',']','{','}',' ',';','"','\t'};
    // Arrayin uzunlu�u delimiters_length adl� de�i�kene at�ld�.
    int delimeters_length   = sizeof(delimiters) / sizeof(delimiters[0]);

    // Program�m�z her d�ng�de bir sat�r okuyup, okunan sat�r� oneLine char arrayine at�yor.
    // D�ng�n�n di�er ad�m�nda bir sonraki sat�r okunuyor. Di�er sat�r okunmadan �nce oneLine i�erisindeki sat�r�n hepsi de�erlendirilip de�erler dosyaya yazd�r�l�yor.
    char oneLine[500] = {0};

    // oneLine i�erisindeki elemanlar� sorgulamak i�in Stack mant���na benzer �ekilde bir y�ntem izledik.
    // Okunan her karakter s�n�rlay�c� bir elemana(delimiter) denk gelinene kadar control i�erisinde depolan�yor.
    // E�er okunan karakter s�n�rlay�c� bir karakter ise control i�erisinde biriktirilen de�erin ne oldu�u sorgulan�yor.
    char control[500] = {0};

    // oneLine ve control�n boyutu manuel ayarlanm��t�r. Dinamik dizi kulland���m�zda istenmeyen durumlar ��kt��� i�in bu �ekilde ayarlanm��t�r.

    // Control i�erisinde ka� adet karakter oldu�unu tutmak i�in count de�i�keni olu�turuldu.
    int count               = 0;

    // Sat�r i�erisindeki de�erler sorgulan�rken s�n�rlay�c� de�erlerden ge�ip ge�medi�imizi ve i�erisinde olup olmad���m�z� anlamak i�in bir�ok flag kullan�ld�.

    bool operatorFlag       = false;
    bool doubleOperatorFlag = false;
    bool spaceFlag          = false;
    bool bracketFlag        = false;
    bool stringFlag         = false;
    bool eol                = false;
    bool commentFlag        = false;

    // fgets ve while d�ng�s�yle her sat�r� okumu� oluyoruz.

    while(fgets(oneLine,sizeof oneLine,psi))
    {
        // Sat�r okunduktan sonra karakter karakter okuma i�lemi bu d�ng� i�erisinde yap�l�yor.
        for(int i = 0; oneLine[i] != '\0'; i++)
        {
            // Art arda gelen ikili operat�r�n kontrol� i�in ex.(++,--,:=)
            if(doubleOperatorFlag)
            {
                doubleOperatorFlag = false;
                continue;
            }

            // Comment i�erisinde olup olmad���m�z� anlamak i�in bir flag kontrol�

            if(commentFlag)
            {
                if(oneLine[i] == ')' && oneLine[i-1] == '*')
                {
                    commentFlag = false;
                }
                continue;
            }



            // Okunan de�erin String olup olmad���na 2 if ile burada kontrol ediyoruz. Flaglar sayesinde ba�latan stringin ne zaman bitti�ini anlay�p yazd�rabiliyoruz.
            if(delimiters[14] == oneLine[i])
            {
                if(stringFlag)
                {
                    fprintf(lex,"%s(%s)\n","String",control);
                    memset(control, 0, sizeof(control));
                    count=0;
                    stringFlag = false;
                    continue;
                }

                else if(!stringFlag)
                {
                    stringFlag = true;
                    continue;
                }
            }

            if(stringFlag)
            {
                control[count] = oneLine[i];
                count++;
                continue;
            }


            // S�n�rlay�c�(Delimiter) karakterlerin ne oldu�unu kontrol etmek i�in bir d�ng� daha kullan�ld�.

            for(int j = 0; j<delimeters_length; j++)
            {

                // Operatorlar�n kontrol� bu if i�erisinde ger�ekle�tiriliyor.
                if((delimiters[j] == oneLine[i]) && (6>j && j>=0))
                {
                    // Delimiter karakter gelince control i�erisindeki de�erin ne oldu�u kontrol ediliyor.
                    if(!checkInteger(control,count,lex) && count != 0)
                    {
                        writeIdentifier(control,count,lex);
                    }

                    // control bo�alt�l�yor.
                    memset(control, 0, sizeof(control));
                    count=0;

                    // Commentin i�erisinden ��kma durumumuza burda bak�yoruz.
                    if( oneLine[i] == '*' && oneLine[i+1] == ')')
                    {
                        commentFlag = false;
                        oneLine[i] = '\0';
                        memset(control, 0, sizeof(control));
                        count=0;
                        break;

                    }

                    // As�l operat�rlerin kesin kararla�t�r�lma i�lemi buradan yap�l�yor.
                    for(int k = 0; k<6; k++)
                    {
                        if(delimiters[k] == oneLine[i+1])
                        {
                            fprintf(lex,"%s(%c%c)\n","Operator",oneLine[i],oneLine[i+1]);

                            operatorFlag = true;
                            doubleOperatorFlag = true;

                            memset(control, 0, sizeof(control));
                            count=0;
                            break;
                        }
                    }
                    if(!doubleOperatorFlag)
                    {
                        fprintf(lex,"%s(%c)\n","Operator",oneLine[i]);
                        operatorFlag = true;
                        memset(control, 0, sizeof(control));
                        count=0;
                        break;
                    }
                }

                // Parantez kontrol� bu if i�erisinde yap�l�yor.
                else if((delimiters[j] == oneLine[i]) && (12>j && j>=6))
                {
                    // Parantez i�erisine girdi�imiz i�in flag true oluyor.
                    bracketFlag = true;
                    if(!checkInteger(control,count,lex) && count != 0)
                    {
                        writeIdentifier(control,count,lex);
                        memset(control, 0, sizeof(control));
                        count=0;
                    }

                    switch(j)
                    {
                    case 6:
                        if(delimiters[2] == oneLine[i+1])
                        {
                            commentFlag = true;
                        }
                        else
                        {
                            fprintf(lex,"%s\n","LeftPar");
                        }
                        break;
                    case 7:
                        fprintf(lex,"%s\n","RightPar");
                        break;
                    case 8:
                        fprintf(lex,"%s\n","LeftSquareBracket");
                        break;
                    case 9:
                        fprintf(lex,"%s\n","RightSquareBracket");
                        break;
                    case 10:
                        fprintf(lex,"%s\n","LeftCurlyBracket");
                        break;
                    case 11:
                        fprintf(lex,"%s\n","RightCurlyBracket");
                        break;
                    }

                    memset(control, 0, sizeof(control));
                    count=0;
                }


                // Space ve tab kontrol�n� burada yap�yoruz
                else if((delimiters[12] == oneLine[i]) || (delimiters[15] == oneLine[i]))
                {
                    spaceFlag = true;

                    int check = checkInteger(control,count,lex);
                    if(!check && count != 0)
                    {
                        writeIdentifier(control,count,lex);
                        memset(control, 0, sizeof(control));
                        count=0;

                    }
                    else if (check)
                    {
                        memset(control, 0, sizeof(control));
                        count=0;
                        break;
                    }
                }

                // Sat�r sonu eleman� i�in kontrol orada yap�l�yor.
                else if(delimiters[13] == oneLine[i])
                {
                    if(!checkInteger(control,count,lex)&& count != 0)
                    {
                        writeIdentifier(control,count,lex);
                        memset(control, 0, sizeof(control));
                        count=0;
                    }
                    eol = true;
                    fprintf(lex,"%s\n","EndOfLine");
                    memset(control, 0, sizeof(control));
                    count=0;
                    break;
                }
                // Delimiter kontrol� i�in olu�turulan for sonu
            }

            // Burada kullan�lan flaglar yaz�lan karakterlerin gelmi� olma durumunda control de�i�keni i�erisine yerle�tirmeden d�ng�n�n ayn� sat�r �zerindeki
            // bir sonraki karaktere ge�ebilmemizi sa�layacak.
            if(spaceFlag)
            {
                spaceFlag = false;
                continue;
            }
            if(operatorFlag)
            {
                operatorFlag = false;
                continue;
            }
            if(bracketFlag)
            {
                bracketFlag = false;
                continue;
            }
            if(commentFlag)
            {
                continue;
            }
            if(eol)
            {
                continue;
            }

            // E�er yap�lan kontrollerden herhangi bir sonu� ��kmam��sa karakterimiz control i�erisine yerle�tirilir.
            control[count] = oneLine[i];
            count++;

            // Karakter yerle�tirildikten sonra control i�erisinde biriken de�erin Keyword olup olmad���n� kontrol ediyoruz.
            if(checkKeyword(control,count,lex))
            {
                memset(control, 0, sizeof(control));
                count=0;
            }
            // Sat�r�n i�erisindekileri okumak i�in olu�turdu�umuz for d�ng�s�n� sonu
        }

        // Yeni sat�ra ge�ece�imiz i�in s�f�rlamam�z gereken de�eleri burada ayarl�yoruz.
        eol = false;
        memset(control, 0, sizeof(control));
        count=0;
        // Sat�rlar� okuma i�lemimiz burada bitiyor.
    }

    // String ve Comment hata kontrollerimiz burada yap�l�yor. E�er de�erler tam anlam�yla olu�turulmam��sa flaglerden anlayarak hata yazd�r�yoruz.
    if( stringFlag == true )
    {
        fprintf(lex,"%s","String constant didn't terminate.");
    }
    if(commentFlag)
    {
        fprintf(lex,"%s","Comment didn't terminate.");
    }
    fclose(psi);

    printf("%s","The program finished successfully.");
    return 0;
}



// Keywordleri kontrol etti�imiz method burada
int checkKeyword(char* control, int count,FILE *lex)
{
    // T�m harfleri k���k yaparak kurala uyduruyoruz.
    for(int x=0; x<count; x++)
    {
        control[x] =tolower(control[x]);
    }
    if(     strcmp(control,"break") == 0|| strcmp(control,"case") == 0|| strcmp(control,"char") == 0|| strcmp(control,"const") == 0||
            strcmp(control,"continue") == 0|| strcmp(control,"do") == 0|| strcmp(control,"else") == 0|| strcmp(control,"enum") == 0||
            strcmp(control,"float") == 0|| strcmp(control,"for") == 0|| strcmp(control,"goto") == 0|| strcmp(control,"if") == 0||
            strcmp(control,"int") == 0|| strcmp(control,"long") == 0|| strcmp(control,"record") == 0|| strcmp(control,"return") == 0||
            strcmp(control,"static") == 0|| strcmp(control,"while")== 0)
    {
        fprintf(lex,"%s(%s)\n","Keyword",control);
        return 1;
    }

    return 0;
}

// Karakterimizin Integer olup olmad���n� bu method ile anl�yoruz.
int checkInteger(char* control, int count,FILE *lex)
{

    int counter = 0;

    for(int i = 0; i<count; i++)
    {
        if((58>(int)control[i]) && ((int)control[i]>47))
        {
            counter++;
        }
    }
    if((counter == count) && counter > 10)
    {
        fprintf(lex,"%s\n","Maximum integer size is 10 digits.");
        return 1;

    }
    else if((counter == count)&& counter !=0)
    {
        fprintf(lex,"%s(","IntConst");
        for(int i = 0 ; i<counter; i++)
        {
            fprintf(lex,"%c",control[i]);
        }
        fprintf(lex,"%s",")\n");

        return 1;
    }
    return 0;
}


// Identifierlar� yazd�rmak i�in bu methodu kullan�yoruz. ��erisinde hata kontrolleri de bulunmaktad�r.
void writeIdentifier(char* control, int count,FILE *lex)
{
    int check = 0;
    for(int x=0; x<count; x++)
    {
        control[x] =toupper(control[x]);
        if(!((91>(int)control[x]) && ((int)control[x]>64)) && !((58>(int)control[x]) && ((int)control[x]>47)) && (int)control[x] != 95)
        {
            check =1 ;
        }
    }

    if(count > MAX_ID)
    {
        fprintf(lex,"%s\n","Error! Maximum identifier size is 20.");
    }
    else if (!((91>(int)control[0]) && ((int)control[0]>64)))
    {
        fprintf(lex,"%s\n","Error! Identifiers start with an alphabetic character.");

    }
    else if(check)
    {
        fprintf(lex,"%s\n","Error! Identifiers  are composed of one or more letters,digits or_ (underscore)");

    }
    else
    {
        fprintf(lex,"%s(","Identifier");
        for(int k = 0; k<count; k++)
        {
            fprintf(lex,"%c",control[k]);
        }
        fprintf(lex,"%s",")\n");
    }

}
