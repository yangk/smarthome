#ifndef DDM_H
#define DDM_H

#include <QString>
#include <QStringList>
#include <stdint.h>

#define DDM_SQLITE

#define DB_FILE_NAME "SmartHome.db"


/* Database */
bool IsDBOpen();
int DDMSetup(void);
void DDMClose();
QString DDMError(void);


struct TestCaseInfo
{
    int id;
    QString tips;
};
int GetTestCaseInfoAll(QList<TestCaseInfo> &testCaseInfos);
int GetTestCaseByID(int id, QString &did, QString &cmd, QString &data);
int AddTestCase(const QString &did, const QString &cmd, const QString &data, const QString &tips);
#if 0
/* User */
int ddmAddUsr(const User &usr);
int ddmGetUsrByIdCardNo(const QString &idCardNo, User *usr);

/* Course */
bool ddmAddCourse(const QString &name, const QString &teacher);
int ddmGetCourseNameByNo(int course_id, QString &name);
int ddmGetCourseNameListByNo(QStringList &list, QList<int> &course_id_list);
int ddmGetCoursesNoByName(const QString &name);
int ddmGetAllCourseID(QList<int> &course_id_list);

/* Paper */
bool ddmAddPaper(const QString &courseName, const QString &paperName);
bool ddmDelPaper(const QString &courseName, const QString &paperName);
bool ddmDelPaperById(int id);
int ddmGetPaperIdByName(const QString &courseName, const QString &paperName);
int ddmGetPapersNameListByCourse(const QString &courseName, QStringList &list);
int ddmGetPapersInfoByCourseID(int course_id, QList<CPaper *> &paper_list);
int ddmGetPaperNameById(int paperid, QString &paperName);

/* Questions */
int ddmAddQuestions(QList<CQuestion> &quesList);
int ddmGetQuestions(QList<CQuestion *> &quesList, int paperId);
bool ddmUpdateQuesAns(int quesId, QString &ans);
bool ddmDelQuesById(int id);
QStringList * ddmGetQuesTypeList();

//会计科目
QStringList *ddmGetAccountTypeList();
QList<AccountEntry> *ddmGetAccountEntryList(bool forceFresh = false);
int ddmAddAccountEntry(QList<AccountEntry> &list);
#endif
#endif // DDM_H
