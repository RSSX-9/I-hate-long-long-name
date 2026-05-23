#include "examsystem.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRandomGenerator>
#include <algorithm>
#include <random>

bool ExamSystem::loadFromResource(const QString &path)
{
    m_questions.clear();

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        loadFallbackQuestions();
        return false;
    }

    const QByteArray data = file.readAll();
    const QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        loadFallbackQuestions();
        return false;
    }

    const QJsonArray array = doc.array();
    for (const QJsonValue &value : array) {
        const QJsonObject obj = value.toObject();
        Question q;
        q.prompt = obj.value(QStringLiteral("prompt")).toString();
        q.answerIndex = obj.value(QStringLiteral("answer")).toInt();
        q.explanation = obj.value(QStringLiteral("explanation")).toString();

        const QJsonArray options = obj.value(QStringLiteral("options")).toArray();
        for (const QJsonValue &opt : options) {
            q.options << opt.toString();
        }

        if (!q.prompt.isEmpty() && q.options.size() >= 2 && q.answerIndex >= 0 && q.answerIndex < q.options.size()) {
            m_questions.push_back(q);
        }
    }

    if (m_questions.isEmpty()) {
        loadFallbackQuestions();
        return false;
    }

    return true;
}

QVector<Question> ExamSystem::randomQuestions(int count) const
{
    QVector<Question> result = m_questions;
    std::mt19937 engine(QRandomGenerator::global()->generate());
    std::shuffle(result.begin(), result.end(), engine);

    if (result.size() > count) {
        result.resize(count);
    }
    return result;
}

int ExamSystem::questionCount() const
{
    return m_questions.size();
}

void ExamSystem::loadFallbackQuestions()
{
    m_questions = {
        {QStringLiteral("C++ 中用于定义类的关键字是？"),
         {QStringLiteral("object"), QStringLiteral("class"), QStringLiteral("define"), QStringLiteral("package")},
         1,
         QStringLiteral("C++ 使用 class 定义类。")},
        {QStringLiteral("Qt 中连接信号和槽常用哪个函数？"),
         {QStringLiteral("connect"), QStringLiteral("listen"), QStringLiteral("bind"), QStringLiteral("join")},
         0,
         QStringLiteral("QObject::connect 用于连接信号与槽。")},
        {QStringLiteral("vector 的 push_back 作用是？"),
         {QStringLiteral("删除元素"), QStringLiteral("末尾添加元素"), QStringLiteral("排序"), QStringLiteral("清空")},
         1,
         QStringLiteral("push_back 会在容器末尾添加元素。")},
        {QStringLiteral("多态通常依赖哪个关键字？"),
         {QStringLiteral("static"), QStringLiteral("virtual"), QStringLiteral("const"), QStringLiteral("friend")},
         1,
         QStringLiteral("virtual 支持运行时多态。")},
        {QStringLiteral("析构函数通常在什么时候调用？"),
         {QStringLiteral("对象销毁时"), QStringLiteral("对象创建前"), QStringLiteral("程序编译时"), QStringLiteral("头文件包含时")},
         0,
         QStringLiteral("对象生命周期结束时会调用析构函数。")}
    };
}
