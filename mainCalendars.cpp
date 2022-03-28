#include "mainCalendars.h"
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include <ctime>
#include <stdexcept>
#include <fstream>
#include <pthread.h>
using namespace std;

// In code months are almost always: 0 - 11 ; Jan - Dic (otherwise it will be specified)
// └ Only user uses 1 - 12

// DATE

Date::Date(int _d, int _m, int _y) {
    validateDate(_d, _m, _y);
    day = _d;
    month = _m;
    year = _y;
}

int Date::getDay() {
    return day;
}

int Date::getMonth() {
    return month;
}

int Date::getYear() {
    return year;
}

string Date::toStringDMY() {
    return (to_string(day) + "/" + to_string(month+1) + "/" + to_string(year));
}

// For RemainderList Map-Coding
string Date::toStringCodeYMD() {
    return (to_string(year) + to_string(month+1) + to_string(day));
}

ostream& operator<<(ostream& os, const Date &_date) {
    os<<(to_string(_date.day) + "/" + to_string(_date.month+1) + "/" + to_string(_date.year));
    return os;
}

// TIME

Time::Time(int _hour, int _minutes) {
    validateTime(_hour, _minutes);
    time.first = _hour;
    time.second = _minutes;
}

pair<int, int> Time::getTime() {
    return time;
}

int Time::getHour() {
    return time.first;
}

int Time::getMinutes() {
    return time.second;
}

string Time::toStringFormatted() {
    string sHour = to_string(time.first), sMinutes = to_string(time.second);
    if(sHour.length() < 2) {
        sHour.push_back(sHour[0]);
        sHour[0] = '0';
    }
    if(sMinutes.length() < 2) {
        sMinutes.push_back(sMinutes[0]);
        sMinutes[0] = '0';
    }
    return (sHour +  ":" + sMinutes);
}

// For RemainderList Map-Coding
string Time::toStringCode() {
    return (to_string(time.first) + to_string(time.second));
}

ostream& operator<<(ostream &os, Time _time) {
    os<<_time.toStringFormatted();
    return os;
}

// CALENDAR

Calendar::Calendar(int _year) {
    validateYear(_year);
    year = _year;
    isLeap = (year%400 == 0 || (year%4 == 0 && year%100 != 0));
}

Calendar::Calendar(Date _markDate) {
    validateYear(_markDate.getYear());
    year = _markDate.getYear();
    mark_date = _markDate;
    isLeap = (year%400 == 0 || (year%4 == 0 && year%100 != 0));
}

int Calendar::month_NumberOfDays(int month) {
    // February
    if(month == 1 && isLeap)
        return 29;
    return number_of_days_month[month];
}

// *Parameters: day, month (both starting from 1)
int Calendar::week_DayNumber(int day, int month) {
    // *DAYS: 0 - 6; Sunday - Saturday
    // Algorithm: (d + m + y + (y/4) + c) mod 7; c = century number
    
    // {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5}: Array with the 1st of all months based on the 1st day of January,
    //  it represents the number of days left obtained by taking: numberOfDays mod numberOfWeeks, numberOfDays and numberOfWeeks of the previous month, so January must start at 0;
    // Leap Year Math adjustment: We add 1 day to every 4th and 400th year (leap years) except the 100th year (is not leap year) 
    // The final array t is obtained bu substracting 1 from each month (each element) starting from February because it adjusts the gap formed because we add an extra day in leap years;
    //  if the value was 0 we take it as 7 and substracting 1 6 (like october)
    vector<int> t = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    int tmpYear = year; 
    tmpYear -= month < 3;
    return (tmpYear + tmpYear/4 - tmpYear/100 + tmpYear/400 + t[month-1] + day) % 7;
}

void Calendar::outCalendar(ostream &out_stream) {
    out_stream<<"          Calendario "<<year<<"\n------------------------------------\n\n";
    // /*EN ->*/ out_stream<<"         Calendar "<<year<<"\n------------------------------------\n\n";
    int weekStartDay = week_DayNumber(1, 1);
    string week_days_labels = "  Dom  Lun  Mar  Mie  Jue  Vie  Sab";
    for(int i=0; i<12; i++) {
        int month_num_days = month_NumberOfDays(i),
            month_name_middle = monthsES[i].length()/2,
            week_days_labels_middle = week_days_labels.length()/2;
        // Center month name
        for(int j=1; j+month_name_middle <= week_days_labels_middle; j++)
            out_stream<<' ';
        out_stream<<monthsES[i]<<'\n';
        out_stream<<week_days_labels<<'\n';
        // /*EN ->*/ out_stream<<monthsEN[i]<<'\n';
        // /*EN ->*/ out_stream<<"  Sun  Mon  Tue  Wed  Thu  Fri  Sat\n";
        int weekJump = 0;
        // skip to starting week day
        while(weekJump < weekStartDay) {
            out_stream<<"     ";
            weekJump++;
        }
        
        for(int j=1; j<=month_num_days; j++) {
            // if its the mark date
            if(mark_date.getMonth() == i && mark_date.getDay() == j) {
                if(j>=10)
                    out_stream<<"  *";
                else
                    out_stream<<"   *";
                out_stream<<j<<'*';
            } // if it passed the mark date and isn't a new row (week)
            else if(mark_date.getMonth() == i && mark_date.getDay() == j-1 && weekJump>0) {
                if(j>=10)
                    out_stream<<"  ";
                else
                    out_stream<<"   ";
                out_stream<<j;
            } else {
                if(j>=10)
                    out_stream<<"   ";
                else
                    out_stream<<"    ";
                out_stream<<j;
            }
            if(weekJump++ == 6) {
                out_stream<<'\n';
                weekJump=0;
            }
        }

        out_stream<<'\n';
        if(weekJump)
            out_stream<<'\n';
        weekStartDay = weekJump;
    }
}

void Calendar::printCalendar() {
    outCalendar(cout);
}

void Calendar::exportCalendar() {
    ofstream file("Calendar_"+to_string(year)+".txt");
    if(file.fail()) {
        cout<<"Error: El archivo no se pudo abrir o crear\n";
        exit(1);
    }

    outCalendar(file);
    file.close();
    cout<<"El calendario ha sido exportado exitosamente\n";
}

// DAILY CALENDAR

DailyCalendar::DailyCalendar(Date _markdate) : Calendar(_markdate) {
    // Limits:
    // 365 -> 5, 73
    // 366 -> 6, 61
    if(isLeap) {
        year_num_days = 366;
        print_row_limit = 61;
    }
}

int DailyCalendar::numOfDaysBeforeToday() {
    int num_days_before_today = 0;
    for(int i=0; i<12; i++) {
        if(mark_date.getMonth() != i)
            num_days_before_today += month_NumberOfDays(i);
        else
            break;
    }
    num_days_before_today += mark_date.getDay() - 1;
    return num_days_before_today;
}

// Days that have passsed(*), today(o) and remaining days of current year(+)
void DailyCalendar::printCalendar() {
    string stitle = "Calendario " + to_string(year) + ": " + to_string(year_num_days) + " dias";
    for(int i=stitle.length()/2; i<print_row_limit/2; i++)
        cout<<' ';
    cout<<stitle<<"\n\n";
    // calc num of current day
    int num_days_before_today = numOfDaysBeforeToday();
    int i;
    for(i=1; i<=num_days_before_today; i++) {
        cout<<'*';
        if(i%print_row_limit == 0)
            cout<<'\n';
    }

    cout<<'o';

    for(i++; i<=year_num_days; i++) {
        cout<<'+';
        if(i%print_row_limit == 0)
            cout<<'\n';
    }
    cout<<"\n- Fecha de hoy: "<<mark_date<<'\n';
    cout<<"- Hoy es el dia "<<num_days_before_today+1<<" del anio\n";
    cout<<"- Faltan "<<(year_num_days - num_days_before_today-1)<<" dias para que el anio termine\n";
}

// REMINDER

Reminder::Reminder(string _txt, Date _date, Time _time) {
    reminder_txt = _txt;
    date = _date;
    time = _time;
}

string Reminder::getText() {
    return reminder_txt;
}

Date Reminder::getDate() {
    return date;
}

Time Reminder::getTime() {
    return time;
}

string Reminder::getDateCode() {
    return date.toStringCodeYMD();
}

string Reminder::getTimeCode() {
    return time.toStringCode();
}

// REMINDER LIST

void RemainderList::addReminder(Reminder rem) {
    string code_date = rem.getDateCode(), code_time = rem.getTimeCode();
    if(!isDatePresent(code_date)) {
        map<string, vector<Reminder>> tmpMap;
        remap.insert({code_date, tmpMap});
        dateDecoder.insert({code_date, rem.getDate().toStringDMY()});
    }
    if(!isDateTimePresent(code_date, code_time)) {
        vector<Reminder> tmpRVec;
        remap[code_date].insert({code_time, tmpRVec});
        timeDecoder.insert({code_time, rem.getTime().toStringFormatted()});
    }
    remap[code_date][code_time].push_back(rem);
}

bool RemainderList::isDatePresent(string code_date) {
    if(remap.find(code_date) != remap.end())
        return true;
    return false;
}

bool RemainderList::isDateTimePresent(string code_date, string code_time) {
    if(!isDatePresent(code_date))
        return false;
    if(remap[code_date].find(code_time) != remap[code_date].end())
        return true;
    return false;
}

bool RemainderList::empty() {
    return remap.empty();
}

map<string, vector<Reminder>> RemainderList::searchDateReminder(Date date) {
    string code_date = date.toStringCodeYMD();
    if(!isDatePresent(code_date)) {
        map<string, vector<Reminder>> tmpMap;
        return tmpMap;
    }
    return remap[code_date];
}

// Not used but implemented
vector<Reminder> RemainderList::searchDateTimeReminder(Date date, Time time) {
    string code_date = date.toStringCodeYMD(), code_time = time.toStringCode();
    if(!isDateTimePresent(code_date, code_time)) {
        vector<Reminder> tmpRVec;
        return tmpRVec;
    }
    return remap[code_date][code_time];
}

void RemainderList::outAllReminders(ostream &out_stream) {
    for(auto it : remap) {
        out_stream<<'\n'<<dateDecoder[it.first]<<'\n';
        for(auto it2 : it.second) {
            out_stream<<"   "<<timeDecoder[it2.first]<<'\n';
            for(auto it3: it2.second) {
                out_stream<<"    └ "<<it3.getText()<<'\n';
            }
        }
    }
}

void RemainderList::printAllReminders() {
    outAllReminders(cout);
}

void RemainderList::exportAllReminders() {
    ofstream file("AllReminders.txt");
    if(file.fail()) {
        cout<<"Error: El archivo no se pudo abrir o crear\n";
        exit(1);
    }
    file<<"Todos los Recordatorios:\n";
    
    outAllReminders(file);

    file.close();
    cout<<"Recordatorios exportados exitosamente\n";
}

// FUNCTIONS

void validateYear(int year) {
    // Gregorian calendar invented in 1582, that year 10 days were lost (special calendar)
    if(year < 1583)
        throw invalid_argument("Anio invalido, anterior a 1583");
}

void validateDate(int day, int month, int year) {
    Calendar helper;
    if(month<0 || month>11)
        throw invalid_argument("Mes invalido");
    if(day<1 || day>helper.month_NumberOfDays(month))
        throw invalid_argument("Dia invalido");
    validateYear(year);
}

void validateTime(int hour, int minutes) {
    if(hour<0 || hour>23)
        throw invalid_argument("Hora invalida");
    if(minutes<0 || minutes>59)
        throw invalid_argument("Minutos invalidos");
}

// PTHREAD FUNCTIONS

void *exportAllRemindersPT(void *remListP) {
    RemainderList *remList = (RemainderList*)remListP;
    remList->exportAllReminders();
    pthread_exit(NULL);
}

void *exportSearchByDateRemindersPT(void *search_resP) {
    map<string, vector<Reminder>> *search_res = (map<string, vector<Reminder>>*)search_resP;
    Date search_date = search_res->begin()->second.begin()->getDate();
    ofstream file("Search_Date_"+search_date.toStringCodeYMD()+".txt");
    Calendar search_rem_calendar(search_date);
    search_rem_calendar.outCalendar(file);
    file<<"\nResultados para "<<search_date.toStringDMY()<<":\n";
    for(auto it : *search_res) {
        for(auto it2 : it.second) {
            file<<"* "<<it2.getTime()<<" -> "<<it2.getText()<<'\n';
        }
    }
    file.close();
    cout<<"Recordatorios exportados exitosamente\n";
    pthread_exit(NULL);
}

void *exportCalendarPT(void *e_calendar) {
    Calendar *export_calendar = (Calendar*)e_calendar;
    export_calendar->exportCalendar();
    pthread_exit(NULL);
}


int main() {
    string opt="";
    unordered_map<string, char> menuValidator = {
        {"<", '<'},
        {">", '>'},
        {"x", 'x'},
        {"1", '1'},
        {"2", '2'},
        {"3", '3'},
        {"4", '4'},
        {"5", '5'}
    };
    time_t todayT = time(0);
    tm *todayLTime = localtime(&todayT);
    // cout<<(todayLTime->tm_wday)<<", "<<(todayLTime->tm_mday)<<'/'<<(todayLTime->tm_mon + 1)<<'/'<<(todayLTime->tm_year + 1900)<<"\n";
    int show_Year = todayLTime->tm_year + 1900;
    Date today_date(todayLTime->tm_mday, todayLTime->tm_mon, todayLTime->tm_year + 1900);
    Calendar main_calendar(today_date);
    RemainderList main_reminders;
    DailyCalendar d_calendar(today_date);

    do{
        // show calendar when program starts
        if(opt.length() == 0)
            main_calendar.printCalendar();
        cout<<"---------------------------------------------------------------\n";
        cout<<"< .- Mostrar calendario de anio anterior\n";
        cout<<"> .- Mostrar calendario de anio posterior\n";
        cout<<"x .- Salir\n";
        cout<<"--------------------------MAS OPCIONES-------------------------\n";
        cout<<"1.- Mostrar calendario con fecha actual\n";
        cout<<"2.- Mostrar calendario de un anio en especifico\n";
        cout<<"3.- MENU Recordatorios\n";
        cout<<"4.- Calendario: Dias que pasaron(*) y restantes del anio actual(+)\n";
        cout<<"5.- Exportar calendario de anio en especifico en .txt\n";
        cout<<"---------------------------------------------------------------\n";
        cin>>opt;
        fflush(stdin);
        cout<<'\n';

        switch(menuValidator[opt]) {
            case '<':
                // past year calendar
                if(--show_Year == today_date.getYear()) {
                    main_calendar = Calendar(today_date);
                    main_calendar.printCalendar();
                } else {
                    try {
                        main_calendar = Calendar(show_Year);
                        main_calendar.printCalendar();
                    } catch(exception &e) {
                        cout<<e.what()<<'\n';
                        show_Year++; // revert --
                    }
                }
                break;
            
            case '>':
                // future year calendar
                if(++show_Year == today_date.getYear()) {
                    main_calendar = Calendar(today_date);
                    main_calendar.printCalendar();
                } else {
                    try {
                        main_calendar = Calendar(show_Year);
                        main_calendar.printCalendar();
                    } catch(exception &e) {
                        cout<<e.what()<<'\n';
                        show_Year--; // revert ++
                    }
                }
                break;

            case 'x':
                cout<<"¿Esta seguro de que desea salir?\n x .- Si\n o .- No\n";
                cin>>opt;
                break;

            // MORE OPT
            case '1':
                // Show current date marked in calendar of current year
                main_calendar = Calendar(today_date);
                main_calendar.printCalendar();
                show_Year = today_date.getYear();
                break;

            case '2': {
                // Specific year calendar
                int desired_year = -1;
                cout<<"¿De que anio desea obtener el calendario?\n";
                cin>>desired_year;
                cout<<'\n';
                try {
                    main_calendar = Calendar(desired_year);
                    main_calendar.printCalendar();
                    show_Year = desired_year;
                } catch(exception &e) {
                    cout<<e.what()<<'\n';
                }
                break;
            }
            
            // MENU RECORDATORIOS
            case '3': {
                string optrem;
                do {
                    cout<<"-----------------------MENU RECORDATORIOS----------------------\n";
                    cout<<"1 .- Mostrar todos los recordatorios\n";
                    cout<<"2 .- Establecer recordatorio (fecha y hora)\n";
                    cout<<"3 .- Buscar recordatorios por fecha\n"; // mostrar y marcarlo en calendario con func de opt 1 de menu anterior
                    cout<<"4 .- Exportar todos los recordatorios en .txt\n"; // in list
                    cout<<"5 .- Buscar y Exportar recordatorios por fecha en .txt\n"; // exportar igual al de optrem 3
                    cout<<"x .- Salir\n";
                    cout<<"---------------------------------------------------------------\n";
                    cin>>optrem;
                    cout<<'\n';

                    switch(menuValidator[optrem]) {
                        case '1':
                            // Print all reminders
                            if(main_reminders.empty()) {
                                cout<<"No hay recordatorios que mostrar\n";
                            } else {
                                cout<<"Todos los Recordatorios:\n";
                                main_reminders.printAllReminders();
                            }
                            break;
                        
                        case '2': {
                            // Add reminder
                            int day, month, year, hour, minutes;
                            string text;
                            cout<<"Establecer recordatorio:\n";
                            cout<<"-Escriba el texto del recordatorio (en 1 linea):\n";
                            getline(cin>>ws, text);
                            cout<<"-Escriba el dia:\n";
                            cin>>day;
                            cout<<"-Escriba el mes (1 - 12):\n";
                            cin>>month;
                            month--;
                            cout<<"-Escriba el anio:\n";
                            cin>>year;
                            cout<<"-Escriba la hora (formato 24-horas):\n";
                            cin>>hour;
                            cout<<"-Escriba los minutos:\n";
                            cin>>minutes;
                            cout<<'\n';
                            try {
                                Reminder newrem(text, Date(day, month, year), Time(hour, minutes));
                                main_reminders.addReminder(newrem);
                                cout<<"-> Recordatorio agregado exitosamente\n";
                            } catch(exception &e) {
                                cout<<e.what()<<'\n';
                            }
                            break;
                        }
                        case '3': {
                            // Search reminders by date
                            if(main_reminders.empty())
                                cout<<"No hay recordatorios que buscar\n";
                            else {
                                int day, month, year;
                                cout<<"Buscar recordatorios por fecha:\n";
                                cout<<"-Escriba el dia:\n";
                                cin>>day;
                                cout<<"-Escriba el mes (1 - 12):\n";
                                cin>>month;
                                month--;
                                cout<<"-Escriba el anio:\n";
                                cin>>year;
                                cout<<'\n';
                                try {
                                    Date search_date(day, month, year);
                                    map<string, vector<Reminder>> search_res = main_reminders.searchDateReminder(search_date);
                                    if(search_res.empty())
                                        cout<<"No se encontro ningun recordatorio en esa fecha\n";
                                    else {
                                        Calendar search_rem_calendar(search_date);
                                        search_rem_calendar.printCalendar();
                                        cout<<"\nResultados para "<<search_date.toStringDMY()<<":\n";
                                        for(auto it : search_res) {
                                            for(auto it2 : it.second) {
                                                cout<<"* "<<it2.getTime()<<" -> "<<it2.getText()<<'\n';
                                            }
                                        }
                                    }
                                } catch(exception &e) {
                                    cout<<e.what()<<'\n';
                                }
                            }
                            break;
                        }
                        case '4': {
                            // Export all reminders in .txt
                            if(main_reminders.empty()) {
                                cout<<"No hay recordatorios que mostrar\n";
                            } else {
                                pthread_t e_thread;
                                int rt;
                                void *status;
                                rt = pthread_create(&e_thread, NULL, exportAllRemindersPT, (void *)&main_reminders);
                                if(rt != 0) {
                                    cout<<"Error: pthread_create failed\n";
                                    exit(EXIT_FAILURE);
                                }
                                pthread_join(e_thread, &status);
                            }
                            break;
                        }
                        case '5': {
                            // Search by date and export reminders in .txt
                            if(main_reminders.empty())
                                cout<<"No hay recordatorios que buscar\n";
                            else {
                                int day, month, year;
                                cout<<"Buscar recordatorios por fecha:\n";
                                cout<<"-Escriba el dia:\n";
                                cin>>day;
                                cout<<"-Escriba el mes (1 - 12):\n";
                                cin>>month;
                                month--;
                                cout<<"-Escriba el anio:\n";
                                cin>>year;
                                cout<<'\n';
                                try {
                                    Date search_date(day, month, year);
                                    map<string, vector<Reminder>> search_res = main_reminders.searchDateReminder(search_date);
                                    if(search_res.empty())
                                        cout<<"No se encontro ningun recordatorio en esa fecha\n";
                                    else {
                                        pthread_t e_thread;
                                        int rt;
                                        void *status;
                                        rt = pthread_create(&e_thread, NULL, exportSearchByDateRemindersPT, (void *)&search_res);
                                        if(rt != 0) {
                                            cout<<"Error: pthread_create failed\n";
                                            exit(EXIT_FAILURE);
                                        }
                                        pthread_join(e_thread, &status);
                                    }
                                } catch(exception &e) {
                                    cout<<e.what()<<'\n';
                                }
                            }
                            break;
                        }
                        case 'x':
                            cout<<"¿Esta seguro de que desea salir?\n x .- Si\n o .- No\n";
                            cin>>optrem;
                            break;
                        
                        default:
                            cout<<"Opcion invalida :(\n";
                    }
                    cout<<'\n';
                    fflush(stdin);
                } while(optrem != "x");
                break;
            }

            case '4': {
                //Calendar: Days that have passsed(*), today(o) and remaining days of current year(+)
                d_calendar.printCalendar();
                break;
            }
            case '5': {
                // Export calendar of specific year in .txt
                int desired_year = -1;
                cout<<"¿De que anio desea exportar el calendario?\n";
                cin>>desired_year;
                cout<<'\n';
                try {
                    pthread_t e_thread;
                    int rt;
                    void *status;
                    Calendar export_calendar(desired_year);
                    rt = pthread_create(&e_thread, NULL, exportCalendarPT, (void *)&export_calendar);
                    if(rt != 0) {
                        cout<<"Error: pthread_create failed\n";
                        exit(EXIT_FAILURE);
                    }
                    pthread_join(e_thread, &status);
                } catch(exception &e) {
                    cout<<e.what()<<'\n';
                }
                break;
            }

            default:
                cout<<"Opcion invalida :(\n";
        }
        cout<<'\n';
        fflush(stdin);
    } while(opt != "x");
    return 0;
}