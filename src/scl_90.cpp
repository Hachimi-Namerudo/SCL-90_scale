#include <qradiobutton.h>
#pragma execution_character_set("utf-8")
#include "scl_90.h"
#include "ui_scl_90.h"
#include "QMessageBox.h"
#include <QTextStream>

//我是弱智吗？

SCL_90::SCL_90(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SCL_90)
{
    ui->setupUi(this);
    QMessageBox::warning(this,"提示","本程序得出的结果仅供参考，该结果不能替代任何专业的医学评估。\n请您根据最近一周以来自己的实际情况，选择最符合您的一项。");
    currentPage = 1;
    ui->pageHint->setText((QString("当前页码：1 / 9")));
    ui->submit->hide();
    dimScore.resize(10);
    ui->Main_table->setAlternatingRowColors(1);
    QString path = ":/questions/Questions.dat";     //应该好了吧
    QFile f(path);
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "提示",
                             QString("源文件读取失败，请检查目录中是否包含Questions.dat\n\n路径：%1\n错误：%2")
                                 .arg(path).arg(f.errorString()));
        return;
    }
    QTextStream txt(&f);
    for(int i = 0;i < 90;i++)
    {
        Question* q = new Question();
        Question::Dimension dim = Question::Default;
        QString dim_t = "";

        txt >> q->description >> dim_t;
        q->dimension = switcher(dim_t);
        questions.push_back(q);
        qDebug() << "Write question" << i + 1 << "successfully,description = " << q->description << ",dimension = " << q->dimension;
    }
    f.close();

    ui->Main_table->setColumnWidth(0,40);
    ui->Main_table->setColumnWidth(1,400);
    ui->Main_table->setColumnWidth(2,70);
    ui->Main_table->setColumnWidth(3,70);
    ui->Main_table->setColumnWidth(4,70);
    ui->Main_table->setColumnWidth(5,70);
    ui->Main_table->setColumnWidth(6,70);

    pull(1);//推送1~10题作为初始内容
}

Question::Dimension SCL_90::switcher(QString str)//大屎山
{
    if(str == "Somatization")
    {
        return Question::Somatization;
    }
    else if(str == "Obsessive")
    {
        return Question::Obsessive;
    }
    else if(str == "Interpersonal")
    {
        return Question::Interpersonal;
    }
    else if(str == "Depression")
    {
        return Question::Depression;
    }
    else if(str == "Anxiety")
    {
        return Question::Anxiety;
    }
    else if(str == "Hostility")
    {
        return Question::Hostility;
    }
    else if(str == "Phobic")
    {
        return Question::Phobic;
    }
    else if(str == "Paranoid")
    {
        return Question::Paranoid;
    }
    else if(str == "Psychotic")
    {
        return Question::Psychotic;
    }
    else if(str == "Additional")
    {
        return Question::Additional;
    }
    else
    {
        qDebug() << "文件存在问题";
        return Question::Default;
    }
}

void SCL_90::debug()
{
    for(int i = 0;i<90;i++)
    {
        qDebug() << "Desc" << questions.at(i)->description << "Dim" << questions.at(i)->dimension;
    }
}

void SCL_90::pull(int index)//index为页码号;向表格推送题目
{
    int num = 10 * (index - 1);
    if(index <= 0)
    {
        qDebug() << "False index" ;
        return;
    }

    ui->Main_table->setRowCount(0);
    //debug();
    for(int i = 0; i < 10;i++)
    {
        ui->Main_table->insertRow(i);//要先插入行才能继续插入内容
        ui->Main_table->setItem(i,0,new QTableWidgetItem(QString::number(num + i + 1)));
        ui->Main_table->setItem(i,1,new QTableWidgetItem(QString(questions.at(i + num)->description)));
        qDebug() << questions.at(i + num)->description;
        QButtonGroup* buttonGroup = new QButtonGroup();//一个按钮组，放在这玩，过两天拿出来炒菜吃。一定要放一点青椒，炒出来更香。
        QRadioButton* non = new QRadioButton("无");
        QRadioButton* sel = new QRadioButton("偶见");
        QRadioButton* mod = new QRadioButton("中等");
        QRadioButton* freq = new QRadioButton("频繁");
        QRadioButton* sev = new QRadioButton("严重");
        buttonGroup->addButton(non,0);
        buttonGroup->addButton(sel,1);
        buttonGroup->addButton(mod,2);
        buttonGroup->addButton(freq,3);
        buttonGroup->addButton(sev,4);
        ui->Main_table->setCellWidget(i,2,non);
        ui->Main_table->setCellWidget(i,3,sel);
        ui->Main_table->setCellWidget(i,4,mod);
        ui->Main_table->setCellWidget(i,5,freq);
        ui->Main_table->setCellWidget(i,6,sev);

        qDebug() << "Call question" << num + i + 1 << "successfully,description = " << questions.at(i + num)->description
                 << ",dimension = " << questions.at(i + num)->dimension << questions.at(i + num)->score;

        if(questions[i + num]->score != 0)//作答就一定有得分罢（
        {
            buttonGroup->button(questions[i + num]->score - 1)->setChecked(1);
        }//一个按钮，一个被创造的按钮，它的勾选状态是持续的。这将说明什么？没有人知道。
    }
}

void SCL_90::push(int index)//将题目作答情况推送至questions数组
{
    int num = 10 * (index - 1);
    for(int i = 0;i < 10;i++)
    {
        for(int j = 0;j < 5;j++)//又是嵌套。
        {
            QRadioButton* bt = static_cast<QRadioButton*>(ui->Main_table->cellWidget(i,j+2));
            if(bt->isChecked() == 1)
            {
                questions.at(i + num)->score = j + 1;
            }
        }
    }
}

void SCL_90::closeEvent(QCloseEvent *e){
    //窗口关闭时弹出的提示窗口
    QMessageBox msgBox;
    msgBox.setText("提示");
    if(ui->stackedWidget->currentIndex() == 0)
    {
        msgBox.setInformativeText("当前数据不会被保存。是否退出？");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();
        if(ret == QMessageBox::Ok)
        {
            e->accept();
        }else
        {
            e->ignore();
        }
    }
}

void SCL_90::calculate()//写完这个就去找纱世里去。这次要抱得紧一点，因为有早八。
{
    for(int i = 0;i < 90;i++)
    {
        totalScore += questions[i]->score;
        if(questions[i]->score >= 2)//阳性计分
        {
            posCount++;
            posScore += double(questions[i]->score);
        }
        dimScore[questions[i]->dimension - 1] += questions[i]->score;
    }
    ui->positive_count->setText(QString::number(posCount));
    ui->positive_sum->setText(QString::number(posScore));
    ui->positive_avr->setText(QString::number(posScore / posCount).first(4));
    ui->negative_count->setText(QString::number(90 - posCount));
    ui->sum->setText(QString::number(totalScore));
    ui->avr->setText(QString::number(totalScore / 90));
    for(int i = 0;i < 10;i++)
    {
        ui->PointTable->setItem(i,0,new QTableWidgetItem(QString::number(dimScore[i])));
        ui->PointTable->setItem(i,1,new QTableWidgetItem(QString::number(dimScore[i] / arg[i])));
        // if((dimScore[i] / arg[i]) >= 2.0)
        // {
        //     ui->PointTable->item(i,0)->setForeground(QBrush(Qt::red));
        //     ui->PointTable->item(i,1)->setForeground(QBrush(Qt::red));
        // }
    }

}

SCL_90::~SCL_90()
{
    delete ui;

    questions.clear();
}

void SCL_90::on_pushButton_3_clicked()
{
    QMessageBox::information(this,"提示","答案没有对错之分，根据你近一周的真实状况填写即可。\n无：近一周内，该症状完全没有出现过，或几乎没有感觉。\n偶现：近一周内，该症状偶尔出现，程度很轻微，不影响或轻微影响日常。\n"
                                           "中等：近一周内，该症状经常出现，程度中等，对日常有一定影响。\n频繁：近一周内，该症状频繁出现，程度较严重，明显影响日常（如学习、工作、社交）。"
                                           "\n严重：近一周内，该症状几乎持续存在，程度严重，严重干扰日常，甚至无法正常生活。");
}

void SCL_90::on_nextPage_clicked()
{
    int num = 10 * (currentPage - 1);
    bool isFinished = 1;
    push(currentPage);
    QString notFin;
    if(currentPage == 9)
    {
        ui->submit->show();
        ui->nextPage->hide();
        ui->hint->setText("已经是最后一页。准备提交结果吗？没事的，放轻松。");
        return;
    }
    else
    {
        for(int i = 0;i < 10;i++)
        {
            bool flag = 0;
            for(int j = 0;j < 5;j++)//又是嵌套。
            {
                if(questions[i + num]->score != 0)
                {
                    flag = 1;
                }
            }
            if(flag == 0)
            {
                notFin.push_back(QString::number(num + i + 1) + ' ');
                if(isFinished == 1)
                {
                    isFinished = 0;
                }
            }
        }
        if(!isFinished)
        {
            QMessageBox::warning(this,"提示",("本页您还有题目未完成，题号为：\n" + notFin + "\n请先完成本页全部题目"));
            ui->hint->setText("有题目未完成");
            return;
        }
        ui->hint->setText("      ");//6个空格
        push(currentPage);
        currentPage++;
        pull(currentPage);
        ui->pageHint->setText((QString("当前页码：%1 / 9")).arg(currentPage));
    }
}


void SCL_90::on_prePage_clicked()
{
    ui->submit->hide();
    ui->nextPage->show();
    if(currentPage == 1)
    {
        ui->hint->setText("已经是第一页");
        return;
    }
    else
    {
        ui->hint->setText("      ");//6个空格
        push(currentPage);
        currentPage--;
        pull(currentPage);
        ui->pageHint->setText((QString("当前页码：%1 / 9")).arg(currentPage));
    }
}

void SCL_90::on_submit_clicked()
{
    bool isFinished = 1;
    push(currentPage);
    QString notFin;
    for(int i = 0;i < 10;i++)
    {
        bool flag = 0;
        for(int j = 0;j < 5;j++)//又是嵌套。
        {
            if(questions[80 + i]->score != 0)
            {
                flag = 1;
            }
        }
        if(flag == 0)
        {
            notFin.push_back(QString::number(90 + i + 1) + ' ');
            if(isFinished == 1)
            {
                isFinished = 0;
            }
        }
    }
    if(!isFinished)
    {
        QMessageBox::warning(this,"提示",("本页您还有题目未完成，题号为：\n" + notFin + "\n请先完成本页全部题目"));
        ui->hint->setText("有题目未完成");
        return;
    }
    QMessageBox::information(this,"提示","提交成功！点击“OK”查看结果。");
    calculate();
    ui->stackedWidget->setCurrentIndex(1);
}



void SCL_90::on_pushButton_clicked()
{
    accept();
}

