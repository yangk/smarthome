#include <QtSql>
#include <QObject>
#include <QTime>
#include <QMessageBox>
#include <QDebug>
#include "ddm.h"
#include "Commfunc.h"

//---------------------------------------------------------------------------------------
static QSqlDatabase db;

//---------------------------------------------------------------------------------------
bool IsDBOpen()
{
	return db.isOpen();
}
//---------------------------------------------------------------------------------------
static void DDMFirstInit(void)
{
    QSqlQuery q;

    /* init the "SmartDevices" table */
    q.exec("DROP TABLE SmartDevices");
    q.exec("CREATE TABLE SmartDevices ("
            "AID INTEGER PRIMARY KEY, "
            "PASSWD INTEGER, "
            "NAME VARCHAR(40), "
            "PANID INTEGER, "
            "GROUP_NO INTEGER,"
            "KEY VARCHAR(40))");
}

int DDMSetup(void)
{
	bool create = !QFile::exists(DB_FILE_NAME);

	db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(DB_FILE_NAME);

	if (db.isOpen())
		return 0;
	if (!db.open())
	{
		QMessageBox::critical(0, QObject::tr("DataBase Error"),
				db.lastError().text());
		return -1;
	}
    if (create) DDMFirstInit();
	return 0;
}

QString DDMError(void)
{
    return db.lastError().text();
}

void DDMClose()
{
	db.close();
	db.removeDatabase("QMYSQL");
}
//---------------------------------------------------------------------------------------
int GetTestCaseInfoAll(QList<TestCaseInfo> &testCaseInfos)
{
    QSqlQuery q;

    QString sql(QObject::tr("SELECT ID, TIPS FROM TestCases"));
    qDebug() << sql;
    if (!q.exec(sql)) {
        qDebug() << db.lastError();
        return -1;
    }

    while (q.next())
    {
        TestCaseInfo testCaseInfo;
        testCaseInfo.id = q.value(0).toInt();
        testCaseInfo.tips = q.value(1).toString();
        testCaseInfos.append(testCaseInfo);
    }
    return 0;
}
int GetTestCaseByID(int id, QString &did, QString &cmd, QString &data)
{
    QSqlQuery q;

    QString sql(QObject::tr("SELECT DID, CMD, DATA "
                            "FROM TestCases "
                            "WHERE ID = %1").arg(id));
    qDebug() << sql;
    if (!q.exec(sql)) {
        qDebug() << db.lastError();
        return -1;
    }

    if (!q.next())
        return -1;

    did = q.value(0).toString();
    cmd = q.value(1).toString();
    data= q.value(2).toString();
    return 0;
}
int AddTestCase(const QString &did, const QString &cmd, const QString &data, const QString &tips)
{
    QString sql(QObject::tr("INSERT INTO TestCases "
                "(DID, CMD, DATA, TIPS) "
                "VALUES ('%1', '%2', '%3', '%4')")
                .arg(did).arg(cmd).arg(data).arg(tips));
    qDebug() << sql;

    QSqlQuery q;
    if (!q.exec(sql)) {
        qDebug() << db.lastError();
        return -1;
    }

    return q.lastInsertId().toInt();
}

#if 0
int ddmAddUsr(const User &usr)
{
    int course_id[3];

    memset(course_id, 0, sizeof(course_id));
    for (int i = 0; i < ARRAY_SZ(course_id) && i < usr.course_id_list.length(); i++) {
        course_id[i] = usr.course_id_list.at(i);
    }

    QString sql(QObject::tr("INSERT INTO users "
                "(name, passwd, IdCardNo, domain, courseId0, courseId1, courseId2) "
                "VALUES ('%1', '%2', '%3', %4, %5, %6, %7)")
                .arg(usr.name).arg(usr.passwd).arg(usr.idCardNo).arg(usr.domain)
                .arg(course_id[0]).arg(course_id[1]).arg(course_id[2]));
    qDebug() << sql;

    QSqlQuery q;
    if (!q.exec(sql)) {
        qDebug() << db.lastError();
        return -1;
    }

    return 0;
}
int ddmGetUsrByIdCardNo(const QString &idCardNo, User *usr)
{
	QSqlQuery q;

    QString sql(QObject::tr("SELECT name, passwd, domain, courseId0, courseId1, courseId2 "
                            "FROM users WHERE IdCardNo = '%1'").arg(idCardNo));
    qDebug() << sql;
    if (!q.exec(sql)) {
        qDebug() << db.lastError();
        return -1;
    }

	if (!q.next())
		return -ERR_USR_NOT_EXIST;

	usr->idCardNo = idCardNo;
	usr->name = q.value(0).toString();
	usr->passwd = q.value(1).toString();
	usr->domain = q.value(2).toInt();
    for (int i = 0; i < 3; i++)
        usr->course_id_list.append(q.value(3+i).toInt());
	return 0;
}

bool ddmAddCourse(const QString &name, const QString &teacher)
{
	QSqlQuery q;

	return q.exec(
			QObject::tr(
					"INSERT INTO courses (name, teacher) VALUES ('%1', '%2')").arg(
					name).arg(teacher));
}
int ddmGetCourseNameByNo(int course_id, QString &name)
{
    QSqlQuery q;

    QString sql(QObject::tr("SELECT name FROM courses WHERE id = %1").arg(course_id));
    qDebug() << sql;
    if (!q.exec(sql)) {
        qDebug() << db.lastError();
        return -1;
    }

    if (q.next()) name = q.value(0).toString();
	return 0;
}
int ddmGetCourseNameListByNo(QStringList &list, QList<int> &course_id_list)
{
	QSqlQuery q;

	QString sql("SELECT name FROM courses ");
    if (course_id_list.length() > 0) {
        sql += QObject::tr("WHERE id = %1 ").arg(course_id_list.at(0));
        for (int i = 1; i < course_id_list.length(); i++)
            sql += QObject::tr("OR id = %1 ").arg(course_id_list.at(i));
	}

    qDebug() << sql;
    if (!q.exec(sql)) {
        qDebug() << db.lastError();
        return -1;
    }
    while (q.next())
		list.append(q.value(0).toString());

	return 0;
}
int ddmGetAllCourseID(QList<int> &course_id_list)
{
    QSqlQuery q;
    QString sql("SELECT id FROM courses");

    qDebug() << sql;
    if (!q.exec(sql)) {
        qDebug() << db.lastError();
        return -1;
    }
    while (q.next())
        course_id_list.append(q.value(0).toInt());

    return 0;
}
int ddmGetCoursesNoByName(const QString &name)
{
	QSqlQuery q;

	q.exec(QObject::tr("SELECT id FROM courses WHERE name = '%1'").arg(name));
	if (q.next())
	{
		return q.value(0).toInt();
	}
	return -1;
}

/* papers */
bool ddmAddPaper(const QString &courseName, const QString &paperName)
{
	QSqlQuery q;

	/* find cordernate course ID */
	int courseID = ddmGetCoursesNoByName(courseName);
	return q.exec(QObject::tr("INSERT INTO papers (courseID, name) "
			"VALUES (%1, '%2')").arg(courseID).arg(paperName));
}
bool ddmDelPaper(const QString &courseName, const QString &paperName)
{
	QSqlQuery q;

	int paperId = ddmGetPaperIdByName(courseName, paperName);
	q.exec(
			QObject::tr("DELETE FROM questions WHERE paperId = %1").arg(
					paperId));

	/* find cordernate course ID */
	int courseID = ddmGetCoursesNoByName(courseName);
	q.exec(
			QObject::tr(
					"DELETE FROM papers WHERE courseID = %1 AND name = '%2'").arg(
					courseID).arg(paperName));

	return 0;
}
bool ddmDelPaperById(int id)
{
	QSqlQuery q;

	q.exec(QObject::tr("DELETE FROM questions WHERE paperId = %1").arg(id));
	q.exec(QObject::tr("DELETE FROM papers WHERE id = %1").arg(id));
	return 0;
}
int ddmGetPapersNameListByCourse(const QString &courseName, QStringList &list)
{
	QSqlQuery q;

	q.exec(
			QObject::tr("SELECT papers.name FROM papers INNER JOIN courses "
					"ON papers.courseID = courses.id AND courses.name = '%1'").arg(
					courseName));
	while (q.next())
	{
		list.append(q.value(0).toString());
	}
	return 0;
}
int ddmGetPapersInfoByCourseID(int course_id, QList<CPaper*> &paper_list)
{
	QSqlQuery q;
    
    QString sql(QObject::tr("SELECT id, name From papers "
                            "WHERE courseID = %1").arg(course_id));
    qDebug() << sql;
    if (!q.exec(sql)) {
        qDebug() << db.lastError();
        return -1;
    }
    while (q.next()) {
        int paper_id = q.value(0).toInt();
        
        CPaper *paper = new CPaper(paper_id);
        paper->name = q.value(1).toString();
        
		paper_list.append(paper);
	}
	return 0;
}
int ddmGetPaperIdByName(const QString &courseName, const QString &paperName)
{
	QSqlQuery q;

	q.exec(
			QObject::tr("SELECT papers.id FROM papers INNER JOIN courses "
					"ON papers.courseID = courses.id "
					"AND courses.name = '%1' AND papers.name = '%2'").arg(
					courseName).arg(paperName));
	if (q.next())
	{
		return q.value(0).toInt();
	}
	return -1;
}
int ddmGetPaperNameById(int paperid, QString &paperName)
{
	QSqlQuery q;

	q.exec(QObject::tr("SELECT name FROM papers WHERE id = %1").arg(paperid));
	if (q.next())
	{
		paperName = q.value(0).toString();
		return 0;
	}
	return -1;
}

//-----------------------------------------------------------------------------------------
// 题目列表
// Add questions
int ddmAddQuestions(QList<CQuestion> &quesList)
{
	QSqlQuery q;
	QString sql;

	db.transaction();
    for (int i = 0; i < quesList.length(); i++) {
        CQuestion ques = quesList[i];

        switch (ques.type) {
        case EQUES_TYPE_SIGOPTION:
        case EQUES_TYPE_MULOPTION:
            sql = QObject::tr("INSERT INTO "
                    "questions (paperId, title, optionA, optionB, optionC, optionD, answer, type) "
                    "VALUES (%1, '%2', '%3', '%4', '%5', '%6', '%7', %8)")
                    .arg(ques.paper_id).arg(ques.title).arg(ques.options[0])
                    .arg(ques.options[1]).arg(ques.options[2]).arg(ques.options[3])
                    .arg(ques.proper_ans).arg(ques.type);
			break;
        case EQUES_TYPE_JUDGE:
        case EQUES_TYPE_CALC_ANALYSIS:
        case EQUES_TYPE_CASE_ANALYSIS:
        case EQUES_TYPE_UNCONFIRM_OPTION:
			sql = QObject::tr("INSERT INTO "
					"questions (paperId, title, answer, type) "
                    "VALUES (%1, '%2', '%3', %4)").arg(ques.paper_id)
                    .arg(ques.title).arg(ques.proper_ans).arg(ques.type);
			break;
        case EQUES_TYPE_ACTUAL_PRACTICE:
            sql = QObject::tr("INSERT INTO "
                    "questions (paperId, title, type) VALUES (%1, '%2', %3)")
                    .arg(ques.paper_id).arg(ques.title).arg(ques.type);
			break;
		}

		if (sql.isEmpty())
			continue;
        if (!q.exec(sql)) {
			QMessageBox::warning(0, QObject::tr("错误"), q.lastError().text());
			db.rollback();
			return -1;
		}
		sql.clear();
	}

    if (!db.commit()) {
		db.rollback();
		return -1;
	}
	return 0;
}

// Find questions
int ddmGetQuestions(QList<CQuestion*> &quesList, int paperId)
{
	QSqlQuery q;

    QString sql(QObject::tr("SELECT id, title, optionA, optionB, optionC, optionD, answer, type "
                            "From questions WHERE paperId = %1").arg(paperId));
    qDebug() << sql;
    if (!q.exec(sql)) {
        qDebug() << db.lastError();
        return -1;
    }

	while (q.next())
	{
        int type = q.value(7).toInt();

        CQuestion *ques = CQuesManager::get_ques((QUES_TYPE)type);
        ques->id = q.value(0).toInt();
        ques->title = q.value(1).toString();
        ques->type = (QUES_TYPE)type;
		for (int i = 0; i < 4; ++i)
		{
			if (q.isNull(2 + i))
				continue;
            ques->options.append(q.value(2 + i).toString());
		}
        ques->proper_ans = q.isNull(6) ? "" : q.value(6).toString();
		quesList.append(ques);
	}

	return 0;
}

// Update question
bool ddmUpdateQuesAns(int quesId, QString &ans)
{
	QSqlQuery q;

	return q.exec(
			QObject::tr("update questions set answer = '%1' where id = %2").arg(
					ans).arg(quesId));
}

// Delete question
bool ddmDelQuesById(int id)
{
	QSqlQuery q;

	return q.exec(QObject::tr("DELETE FROM questions WHERE id = %1").arg(id));
}

//-----------------------------------------------------------------------------------------
//会计科目列表
QStringList * ddmGetAccountTypeList()
{
	static QStringList list;

	if (list.length() == ACCOUNT_TYPE_NUM)
		return &list;
	list << QObject::tr("资产类") << QObject::tr("负债类") << QObject::tr("共同类")
			<< QObject::tr("所有者权益类") << QObject::tr("成本类")
			<< QObject::tr("损益类");
	return &list;
}
QList<AccountEntry> *ddmGetAccountEntryList(bool forceFresh)
{
	static QList<AccountEntry> list;

	if (forceFresh)
		list.clear();

	if (list.length() > 0)
		return &list;

	QSqlQuery q;

	q.exec(QObject::tr("SELECT name, type From account_entrys"));
	while (q.next())
	{
		AccountEntry entry;
		entry.name = q.value(0).toString();
		entry.type = q.value(1).toInt();
		list.append(entry);
	}
	return &list;
}
int ddmAddAccountEntry(QList<AccountEntry> &list)
{
	QSqlQuery q;

	db.transaction();
	q.exec(QObject::tr("DELETE FROM account_entrys"));
	for (int i = 0; i < list.length(); i++)
	{
		AccountEntry entry = list[i];
		q.exec(
				QObject::tr(
						"INSERT INTO account_entrys (name, type) VALUES ('%1', %2)").arg(
						entry.name).arg(entry.type));
	}
	if (!db.commit())
	{
		db.rollback();
		return -1;
	}
	return 0;
}
#endif
//-----------------------------------------------------------------------------------------
