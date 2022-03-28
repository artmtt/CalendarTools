#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
using namespace std;

void validateYear(int);
void validateDate(int, int, int);
void validateTime(int, int);
void *exportCalendarPT(void *e_calendar);
void *exportSearchByDateRemindersPT(void *obj);
void *exportCalendarPT(void *e_calendar);

class Date {
    private:
        // *MONTHS: 0 - 11 ; Jan - Dic
        int day=-1, month=-1, year=-1;
    public:
        Date(){}

        Date(int _d, int _m, int _y);

        int getDay();

        int getMonth();

        int getYear();

        string toStringDMY();

        // For RemainderList Map-Coding
        string toStringCodeYMD();

        friend ostream& operator<<(ostream& os, const Date &_date);
};


// Using 24-HOUR TIME
class Time {
    private:
        pair<int, int> time;
    public:
        Time(){}

        Time(int _hour, int _minutes);

        pair<int, int> getTime();

        int getHour();

        int getMinutes();

        string toStringFormatted();

        // For RemainderList Map-Coding
        string toStringCode();

        friend ostream& operator<<(ostream &os, Time _time);
};


class Calendar {
    private:
        // *MONTHS: 0 - 11 ; Jan - Dic 
        vector<string> monthsES = {"Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"};
        // EN -> vector<string> monthsEN = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
        vector<int> number_of_days_month = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    protected:
        int year=-1;
        Date mark_date;
        bool isLeap=false;

    public:
        Calendar(){}

        Calendar(int _year);

        Calendar(Date _markDate);

        int month_NumberOfDays(int month);

        // *Parameters: day, month (both starting from 1)
        int week_DayNumber(int day, int month);

        void outCalendar(ostream &out_stream);

        virtual void printCalendar();

        void exportCalendar();
};


class DailyCalendar: public Calendar {
    private:
        int year_num_days=365, print_row_limit=73;

    public:
        DailyCalendar(){}

        DailyCalendar(Date _markdate);

        int numOfDaysBeforeToday();

        // Days that have passsed(*), today(o) and remaining days of current year(+)
        void printCalendar();
};


class Reminder {
    private:
        string reminder_txt="";
        Date date;
        Time time;

    public:
        Reminder(){}

        Reminder(string _txt, Date _date, Time _time);

        string getText();

        Date getDate();

        Time getTime();

        string getDateCode();

        string getTimeCode();
};


class RemainderList {
    private:
        // Map: key->date_code, value->(Map: key->time_code, value->vector<Reminder>)  
        map<string, map<string, vector<Reminder>>> remap;
        map<string, string> dateDecoder, timeDecoder;
    public:
        RemainderList(){}

        void addReminder(Reminder rem);

        bool isDatePresent(string code_date);

        bool isDateTimePresent(string code_date, string code_time);

        bool empty();

        map<string, vector<Reminder>> searchDateReminder(Date date);

        // Not used but implemented
        vector<Reminder> searchDateTimeReminder(Date date, Time time);

        void outAllReminders(ostream &out_stream);

        void printAllReminders();

        void exportAllReminders();
};
