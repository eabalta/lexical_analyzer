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

    // Okunan kodda sýnýrlama yapan tüm deðerler delimiters içinde toplandý.
    char delimiters[]       = {'+','-','*','/',':','=','(',')','[',']','{','}',' ',';','"','\t'};
    // Arrayin uzunluðu delimiters_length adlý deðiþkene atýldý.
    int delimeters_length   = sizeof(delimiters) / sizeof(delimiters[0]);

    // Programýmýz her döngüde bir satýr okuyup, okunan satýrý oneLine char arrayine atýyor.
    // Döngünün diðer adýmýnda bir sonraki satýr okunuyor. Diðer satýr okunmadan önce oneLine içerisindeki satýrýn hepsi deðerlendirilip deðerler dosyaya yazdýrýlýyor.
    char oneLine[500] = {0};

    // oneLine içerisindeki elemanlarý sorgulamak için Stack mantýðýna benzer þekilde bir yöntem izledik.
    // Okunan her karakter sýnýrlayýcý bir elemana(delimiter) denk gelinene kadar control içerisinde depolanýyor.
    // Eðer okunan karakter sýnýrlayýcý bir karakter ise control içerisinde biriktirilen deðerin ne olduðu sorgulanýyor.
    char control[500] = {0};

    // oneLine ve controlün boyutu manuel ayarlanmýþtýr. Dinamik dizi kullandýðýmýzda istenmeyen durumlar çýktýðý için bu þekilde ayarlanmýþtýr.

    // Control içerisinde kaç adet karakter olduðunu tutmak için count deðiþkeni oluþturuldu.
    int count               = 0;

    // Satýr içerisindeki deðerler sorgulanýrken sýnýrlayýcý deðerlerden geçip geçmediðimizi ve içerisinde olup olmadýðýmýzý anlamak için birçok flag kullanýldý.

    bool operatorFlag       = false;
    bool doubleOperatorFlag = false;
    bool spaceFlag          = false;
    bool bracketFlag        = false;
    bool stringFlag         = false;
    bool eol                = false;
    bool commentFlag        = false;

    // fgets ve while döngüsüyle her satýrý okumuþ oluyoruz.

    while(fgets(oneLine,sizeof oneLine,psi))
    {
        // Satýr okunduktan sonra karakter karakter okuma iþlemi bu döngü içerisinde yapýlýyor.
        for(int i = 0; oneLine[i] != '\0'; i++)
        {
            // Art arda gelen ikili operatörün kontrolü için ex.(++,--,:=)
            if(doubleOperatorFlag)
            {
                doubleOperatorFlag = false;
                continue;
            }

            // Comment içerisinde olup olmadýðýmýzý anlamak için bir flag kontrolü

            if(commentFlag)
            {
                if(oneLine[i] == ')' && oneLine[i-1] == '*')
                {
                    commentFlag = false;
                }
                continue;
            }



            // Okunan deðerin String olup olmadýðýna 2 if ile burada kontrol ediyoruz. Flaglar sayesinde baþlatan stringin ne zaman bittiðini anlayýp yazdýrabiliyoruz.
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


            // Sýnýrlayýcý(Delimiter) karakterlerin ne olduðunu kontrol etmek için bir döngü daha kullanýldý.

            for(int j = 0; j<delimeters_length; j++)
            {

                // Operatorlarýn kontrolü bu if içerisinde gerçekleþtiriliyor.
                if((delimiters[j] == oneLine[i]) && (6>j && j>=0))
                {
                    // Delimiter karakter gelince control içerisindeki deðerin ne olduðu kontrol ediliyor.
                    if(!checkInteger(control,count,lex) && count != 0)
                    {
                        writeIdentifier(control,count,lex);
                    }

                    // control boþaltýlýyor.
                    memset(control, 0, sizeof(control));
                    count=0;

                    // Commentin içerisinden çýkma durumumuza burda bakýyoruz.
                    if( oneLine[i] == '*' && oneLine[i+1] == ')')
                    {
                        commentFlag = false;
                        oneLine[i] = '\0';
                        memset(control, 0, sizeof(control));
                        count=0;
                        break;

                    }

                    // Asýl operatörlerin kesin kararlaþtýrýlma iþlemi buradan yapýlýyor.
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

                // Parantez kontrolü bu if içerisinde yapýlýyor.
                else if((delimiters[j] == oneLine[i]) && (12>j && j>=6))
                {
                    // Parantez içerisine girdiðimiz için flag true oluyor.
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


                // Space ve tab kontrolünü burada yapýyoruz
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

                // Satýr sonu elemaný için kontrol orada yapýlýyor.
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
                // Delimiter kontrolü için oluþturulan for sonu
            }

            // Burada kullanýlan flaglar yazýlan karakterlerin gelmiþ olma durumunda control deðiþkeni içerisine yerleþtirmeden döngünün ayný satýr üzerindeki
            // bir sonraki karaktere geçebilmemizi saðlayacak.
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

            // Eðer yapýlan kontrollerden herhangi bir sonuç çýkmamýþsa karakterimiz control içerisine yerleþtirilir.
            control[count] = oneLine[i];
            count++;

            // Karakter yerleþtirildikten sonra control içerisinde biriken deðerin Keyword olup olmadýðýný kontrol ediyoruz.
            if(checkKeyword(control,count,lex))
            {
                memset(control, 0, sizeof(control));
                count=0;
            }
            // Satýrýn içerisindekileri okumak için oluþturduðumuz for döngüsünü sonu
        }

        // Yeni satýra geçeceðimiz için sýfýrlamamýz gereken deðeleri burada ayarlýyoruz.
        eol = false;
        memset(control, 0, sizeof(control));
        count=0;
        // Satýrlarý okuma iþlemimiz burada bitiyor.
    }

    // String ve Comment hata kontrollerimiz burada yapýlýyor. Eðer deðerler tam anlamýyla oluþturulmamýþsa flaglerden anlayarak hata yazdýrýyoruz.
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



// Keywordleri kontrol ettiðimiz method burada
int checkKeyword(char* control, int count,FILE *lex)
{
    // Tüm harfleri küçük yaparak kurala uyduruyoruz.
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

// Karakterimizin Integer olup olmadýðýný bu method ile anlýyoruz.
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


// Identifierlarý yazdýrmak için bu methodu kullanýyoruz. Ýçerisinde hata kontrolleri de bulunmaktadýr.
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
