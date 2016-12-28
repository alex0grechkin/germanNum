#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qmessagebox.h"
#include <set>

enum TGroup { numberOfSingleFormat, numberFormat10_19, numberOfDozensOfFormats };
enum TWords { null, eins, zwei, drei, vier, funf, sechs, sieben, acht, neun, zehn, elf, zwolf, dreizehn, vierzehn, funfzehn, sechzehn, siebzehn, achtzehn, neunzehn, ein, und, hundert, zwanzig, dreibig, vierzig, funfzig, sechzig, siebzig, achtzig, neunzig, empty };
enum TStarSlovWords { az, wedy, glagol, dobro, est, zelo, zemlya, ije, fita, i, kako, ludy, mislete, nash, ksi, on_, pokoi, cherv, rcy, slova, tverd, uk, fert, ha, psy, o, cy };

struct TWord {
    TWords Word;
    int pos;
    std::set<TGroup> Group;
};

/*!
  \brief Класс парсер немецких числительных
  \author Гречкин А.Э.
  \version 1.0
  \date Декабрь 2016 года

  Данный класс предназначен для поиска немецких числительных в строке QString,
  которые могут быть представлены в арабских цифрах и старословянских числительных.
  Поиск выполняется в соответствии с деревом разбора, в случае ошибки последовательности
  использования слов генерируется соотвествующее текстовое сообщение в поле errorMessage.
*/

class Parser
{
    QString TargetString;
    std::vector<TWord> FindWords;
    std::vector<QString> FindWordsStr;
    std::map<TWords, QString> strWords;
    std::map<TStarSlovWords, QString> strStarSlovWords;
    std::map<TGroup, QString> strGroups;
    std::map<TGroup, QString> strAfterGroups;
    std::map<TWords, int> intWords;
    std::map<TStarSlovWords, int> intStarSlovWords;
    std::map<TGroup, std::set<TWords>> Groups;

public:
    int num;
    QString errorMessage;
    Parser(QString str);
    ~Parser();
    bool parse();
    QString getTargetString();
    void setTargetString(QString newstr);
    void showWords();
    bool runInTree();
    QString getStarSlov();
};

/*!
Конструктор
\param[in] str Исходная строка поиска немецких числительных
\return Parser
*/

Parser::Parser(QString str)
{
    TargetString = str;
        strWords = { {null, QString("null")},
                    { eins, QString("eins") },
                    { zwei, QString("zwei") },
                    { drei, QString("drei") },
                    { vier, QString("vier") },
                    { funf, QString("funf") },
                    { sechs, QString("sechs") },
                    { sieben, QString("sieben") },
                    { acht, QString("acht") },
                    { neun, QString("neun") },
                    { zehn, QString("zehn") },
                    { elf, QString("elf") },
                    { zwolf, QString("zwolf") },
                    { dreizehn, QString("dreizehn") },
                    { vierzehn, QString("vierzehn") },
                    { funfzehn, QString("funfzehn") },
                    { sechzehn, QString("sechzehn") },
                    { siebzehn, QString("siebzehn") },
                    { achtzehn, QString("achtzehn") },
                    { neunzehn, QString("neunzehn") },
                    { ein, QString("ein") },
                    { und, QString("und") },
                    { hundert, QString("hundert") },
                    { zwanzig, QString("zwanzig") },
                    { dreibig, QString("dreibig") },
                    { vierzig, QString("vierzig") },
                    { funfzig, QString("funfzig") },
                    { sechzig, QString("sechzig") },
                    { siebzig, QString("siebzig") },
                    { achtzig, QString("achtzig") },
                    { neunzig, QString("neunzig") }
        };
        intWords = { {null, 0},
                        { eins, 1},
                        { zwei, 2},
                        { drei, 3},
                        { vier, 4},
                        { funf, 5},
                        { sechs, 6},
                        { sieben, 7},
                        { acht, 8},
                        { neun, 9},
                        { zehn, 10},
                        { elf, 11},
                        { zwolf, 12},
                        { dreizehn, 13},
                        { vierzehn, 14},
                        { funfzehn, 15},
                        { sechzehn, 16},
                        { siebzehn, 17},
                        { achtzehn, 18},
                        { neunzehn, 19},
                        { ein, 1},
                        { und, 0},
                        { hundert, 0},
                        { zwanzig, 20},
                        { dreibig, 30},
                        { vierzig, 40},
                        { funfzig, 50},
                        { sechzig, 60},
                        { siebzig, 70},
                        { achtzig, 80},
                        { neunzig, 90}
            };
        Groups = { { numberOfSingleFormat , { null,eins,ein,zwei,drei,vier,funf,sechs,sieben,acht,neun }},
                        { numberFormat10_19 ,{ zehn, elf, zwolf, dreizehn, vierzehn, funfzehn, sechzehn, siebzehn, achtzehn, neunzehn }},
                        { numberOfDozensOfFormats ,{ zwanzig, dreibig, vierzig, funfzig, sechzig, siebzig, achtzig, neunzig }}
            };
        strGroups = { { numberOfSingleFormat , QString("число едениченого формата")},
                            { numberFormat10_19 , QString("число формата 10-19") },
                            { numberOfDozensOfFormats , QString("число десятичного формата") }
            };
        strAfterGroups = { { numberOfSingleFormat , QString("числа еденичного формата") },
                                { numberFormat10_19 , QString("числа формата 10-19") },
                                { numberOfDozensOfFormats , QString("числа десятичного формата") }
            };
        strStarSlovWords = { { az , QString("аз") },
                                { wedy , QString("веди") },
                                { glagol , QString("глаголь") },
                                { dobro , QString("добро") },
                                { est , QString("есть") },
                                { zelo , QString("зело") },
                                { zemlya , QString("земля") },
                                { ije , QString("иже") },
                                { fita , QString("фита") },
                                { i , QString("и") },
                                { kako , QString("како") },
                                { ludy , QString("люди") },
                                { mislete , QString("мыслете") },
                                { nash , QString("наш") },
                                { ksi , QString("кси") },
                                { on_ , QString("он") },
                                { pokoi , QString("покой") },
                                { cherv , QString("червь") },
                                { rcy , QString("рцы") },
                                { slova , QString("слова") },
                                { tverd , QString("твердь") },
                                { uk , QString("ук") },
                                { fert , QString("ферт") },
                                { ha , QString("ха") },
                                { psy , QString("пси") },
                                { o , QString("о") },
                                { cy , QString("цы") }
            };
        intStarSlovWords = { { az , 1 },
                                { wedy , 2 },
                                { glagol , 3 },
                                { dobro , 4 },
                                { est , 5 },
                                { zelo , 6 },
                                { zemlya , 7 },
                                { ije , 8 },
                                { fita , 9 },
                                { i , 10 },
                                { kako , 20 },
                                { ludy , 30 },
                                { mislete , 40 },
                                { nash , 50 },
                                { ksi , 60 },
                                { on_ , 70 },
                                { pokoi , 80 },
                                { cherv , 90 },
                                { rcy , 100 },
                                { slova , 200 },
                                { tverd , 300 },
                                { uk , 400 },
                                { fert , 500 },
                                { ha , 600 },
                                { psy , 700 },
                                { o , 800 },
                                { cy , 900 }
            };
}

/*!
Деструктор
*/

Parser::~Parser()
{
    FindWords.clear();
    FindWordsStr.clear();
}

/*!
Возвращает текущее состояние исходной строки поиска
\return  QString TargetString
*/

QString Parser::getTargetString()
{
    return TargetString;
}

/*!
Изменяет текущее состояние исходной строки поиска
\param[in] newstr Исходная строка поиска немецких числительных
*/


void Parser::setTargetString(QString newstr)
{
    TargetString = newstr;
}


/*!
Выполняет поиск немецких числительных с последующей записью в контейнер FindWords в порядке следования
\return  False - исходная строка поиска содержит не входящие в алфавит слова, True - поиск слов выполнен успешно.
*/

bool  Parser::parse()
{
    bool result = true;
    bool findFlag;
    QString nextWord="";
    for (int j = 0; j < TargetString.length(); j++)
    {
        if (TargetString[j] != ' ')
            nextWord += TargetString[j];
        if (TargetString[j] == ' ' || j == TargetString.length() - 1)
            if (nextWord.length() != 0)
            {
                FindWordsStr.push_back(nextWord);
                nextWord = "";
            }
    }

    for (unsigned int j = 0; j < FindWordsStr.size(); j++)
    {
        findFlag = false;
        TWord FindWord;
        for (int i = (int)eins; i <= neunzig; i++)
        {
            QString tmp1 = *(FindWordsStr.begin() + j);
            QString tmp2 = strWords.at((TWords)i);
            if (tmp1 == tmp2)
            {
                FindWord.Word = (TWords)i;
                FindWords.push_back(FindWord);
                findFlag = true;
                break;
            }
        }
        if (!findFlag)
        {
            errorMessage = "не опознанное слово ";
            errorMessage += *(FindWordsStr.begin() + j);
            return false;
        }

    }
    if (FindWords.size() == 0)

        result = false;
    return result;
}

/*!
Отображает найденные слова содержащиеся в контейнере FindWords в порядке следования
*/

void Parser::showWords()
{

    for (unsigned int i = 0; i < FindWords.size(); i++){
        QMessageBox msg;
                msg.setText(strWords.at((FindWords.begin() + i)->Word));
        msg.exec();
    }
}

/*!
  Разбор найденных слов в соответствии с деревом разбора.
  \return  True - последовательность слов не нарушена, False - имеются ошибки последовательности слов.
*/

bool Parser::runInTree()
{
    TWords currentWord;
    bool hundertFlag = false;
    bool result = true;
    num = 0;
    errorMessage = "";
    for (unsigned int i = 0; i < FindWords.size(); i++)
    {

        currentWord = ((FindWords.begin() + i)->Word);
        if (Groups.at(numberOfSingleFormat).find(currentWord) != Groups.at(numberOfSingleFormat).end())
        {
            num += intWords.at(currentWord);
            if (FindWords.size() - 1 != i)
            {
                if ((FindWords.begin() + i + 1)->Word == hundert)
                {
                    if (!hundertFlag)
                    {
                        hundertFlag = true;
                        num *= 100;
                        continue;
                    }
                    else
                    {
                        errorMessage = "hundert не может идти ";
                        errorMessage += strAfterGroups.at(numberOfSingleFormat);
                        errorMessage += " повторно";
                        return false;
                    }
                }




                if ((FindWords.begin() + i + 1)->Word == und)
                    continue;
                if (Groups.at(numberOfSingleFormat).find((FindWords.begin() + i + 1)->Word) != Groups.at(numberOfSingleFormat).end())
                    errorMessage = strGroups.at(numberOfSingleFormat);
                if (Groups.at(numberFormat10_19).find((FindWords.begin() + i + 1)->Word) != Groups.at(numberFormat10_19).end())
                    errorMessage = strGroups.at(numberFormat10_19);
                if (Groups.at(numberOfDozensOfFormats).find((FindWords.begin() + i + 1)->Word) != Groups.at(numberOfDozensOfFormats).end())
                    errorMessage = strGroups.at(numberOfDozensOfFormats);
                errorMessage += " не может идти после ";
                errorMessage += strAfterGroups.at(numberOfSingleFormat);
                return false;
            }
            else
                continue;
        }

        if (Groups.at(numberFormat10_19).find(currentWord) != Groups.at(numberFormat10_19).end())
        {
            num += intWords.at(currentWord);
            if (FindWords.size() - 1 != i)
            {
                if (Groups.at(numberOfSingleFormat).find((FindWords.begin() + i + 1)->Word) != Groups.at(numberOfSingleFormat).end())
                    errorMessage = strGroups.at(numberOfSingleFormat);
                if (Groups.at(numberFormat10_19).find((FindWords.begin() + i + 1)->Word) != Groups.at(numberFormat10_19).end())
                    errorMessage = strGroups.at(numberFormat10_19);
                if (Groups.at(numberOfDozensOfFormats).find((FindWords.begin() + i + 1)->Word) != Groups.at(numberOfDozensOfFormats).end())
                    errorMessage = strGroups.at(numberOfDozensOfFormats);
                if ((FindWords.begin() + i + 1)->Word == hundert)
                    errorMessage = strWords.at(hundert);
                if ((FindWords.begin() + i + 1)->Word == und)
                    errorMessage = strWords.at(und);
                errorMessage += " не может идти после ";
                errorMessage += strAfterGroups.at(numberFormat10_19);
                return false;
            }
            else
                continue;
        }

        if (Groups.at(numberOfDozensOfFormats).find(currentWord) != Groups.at(numberOfDozensOfFormats).end())
        {
            num += intWords.at(currentWord);
            if (FindWords.size() - 1 != i)
            {
                if (Groups.at(numberOfSingleFormat).find((FindWords.begin() + i + 1)->Word) != Groups.at(numberOfSingleFormat).end())
                    errorMessage = strGroups.at(numberOfSingleFormat);
                if (Groups.at(numberFormat10_19).find((FindWords.begin() + i + 1)->Word) != Groups.at(numberFormat10_19).end())
                    errorMessage = strGroups.at(numberFormat10_19);
                if (Groups.at(numberOfDozensOfFormats).find((FindWords.begin() + i + 1)->Word) != Groups.at(numberOfDozensOfFormats).end())
                    errorMessage = strGroups.at(numberOfDozensOfFormats);
                if ((FindWords.begin() + i + 1)->Word == hundert)
                    errorMessage = strWords.at(hundert);
                if ((FindWords.begin() + i + 1)->Word == und)
                    errorMessage = strWords.at(und);
                errorMessage += " не может идти после ";
                errorMessage += strAfterGroups.at(numberOfDozensOfFormats);
                return false;
            }
            else
                continue;
        }

        if (currentWord == hundert && i > 0)
        {
            if (FindWords.size() - 1 != i)
            {
                if ((FindWords.begin() + i + 1)->Word == hundert)
                {
                    errorMessage = "после hundert не может идти hundert";
                    return false;
                }
                if ((FindWords.begin() + i + 1)->Word == und)
                {
                    errorMessage = "после hundert не может идти und";
                    return false;
                }
            }
            continue;
        }

        if (currentWord == und && i > 0)
        {
            if (FindWords.size() - 1 != i)
                if (Groups.at(numberOfDozensOfFormats).find((FindWords.begin() + i + 1)->Word) != Groups.at(numberOfDozensOfFormats).end())
                    continue;
            errorMessage = "после und ожидалось ";
            errorMessage+=strGroups.at(numberOfDozensOfFormats);
            return false;
        }
        errorMessage = "в начале не может стоять ";
        errorMessage += strWords.at(currentWord);
            return false;

    }
    return result;
}

/*!
  Конвертирует обнаруженные немецкие числительные в старословянские.
  \return  QString строка содержащая старословянские числительные.
*/

QString Parser::getStarSlov()
{
    bool flag = false;
    int sot = num / 100 * 100;
    int des = (num - sot) / 10 * 10;
    int ed = num - sot - des;
    if ((des == 10) && (ed > 0) && (ed < 10))
        flag = true;
    QString result = "";
    QString strdes = "";
    QString stred = "";
    for (int i = (int)az; i <=(int)cy; i++)
    {
        if (intStarSlovWords.at((TStarSlovWords)i) == sot)
            result += strStarSlovWords.at((TStarSlovWords)i);
        if (intStarSlovWords.at((TStarSlovWords)i) == des)
            strdes += strStarSlovWords.at((TStarSlovWords)i);
        if (intStarSlovWords.at((TStarSlovWords)i) == ed)
            stred += strStarSlovWords.at((TStarSlovWords)i);
    }
    if (flag)
        result += stred + ' ' + strdes;
    else
        result += strdes + ' ' + stred;
    return result;
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
        QMessageBox msg;
        QString tmp="";
        tmp = ui->lineEdit->text();
        Parser parser(tmp);

        if (parser.parse())
        {
            parser.showWords();
            if (parser.runInTree())
            {
                ui->lineEdit_2->setText(QString::number(parser.num));
                ui->lineEdit_3->setText(parser.getStarSlov());

            }
            else
            {
                ui->lineEdit_2->setText("");
                ui->lineEdit_3->setText("");
                msg.setText(parser.errorMessage);
            }
        }
        else
        {
            ui->lineEdit_2->setText("");
            ui->lineEdit_3->setText("");
            msg.setText(parser.errorMessage);
        }
}
