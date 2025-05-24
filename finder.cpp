#include "Header.h" 
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

size_t b3 = 0; 
using namespace std;

//Функция открытия и чтения pcm файла (на вход путь к файлу)
vector<float> open_file(const string& filename)
{
    // Открытие файла 
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Не удалось открыть файл." << endl;
    }

    // Чтение данных из файла
    vector<float> samples;
    float sample;
    while (file.read(reinterpret_cast<char*>(&sample), sizeof(float))) //Записывай 32 бита (размер float) по адресу переменной sample, но представь, что ты читаешь char, а не float т. к. функция ожидает char 
    {
        samples.push_back(sample); //Добавляем значение sample в конец массива samples
    }
    file.close();

    return samples;
}

//Функция вычисления среднего значения уровня амплитуды на отрезках по gap семплов
vector<float> mid_gap(const vector<float>& samples, int small_gap)
{
    vector <float> mid_val_gap; //Массив, где будем хранить средние значения по отрезку gap
    float sum_gap_samples = 0;
    int temp = 0;
    size_t num_samples = samples.size(); //Число семплов в файле    

    for (int i = 0; i < num_samples; i += small_gap) //Цикл, который считает средние значения  амплитуды за gap значений 
    {
        sum_gap_samples = 0;

        for (int j = i; j < i + small_gap; j++)//Складываем gap значений 
        {
            sum_gap_samples += samples[j];
        }

        mid_val_gap.push_back(sum_gap_samples / small_gap); //Получаем среднее значение за gap значений и сохраняем его в массив 
    }

    return mid_val_gap;
}

//Функция вычисления среднего значения уровня амплитуды на отрезках по gap семплов со сдвигом на small_gap. Например, считаем среднее на 100 отсчётах, сначала с 0 до 100, затем с 10 до 110, с 20 до 120 и т.д.
vector<float> mid_gap_shift(const vector<float>& samples, int small_gap, int big_gap)
{
    if (big_gap > small_gap)
    {
        int temp = 0;
        size_t num_samples = samples.size(); //Число семплов в файле  
        vector <float> mid_gap_sh; //Массив, где будем хранить средние значения по отрезку gap 
        float sum_gap_samples = 0;

        for (int i = 0; i <= num_samples - big_gap; i += small_gap) //Цикл, который считает средние значения  амплитуды за gap значений
        {
            sum_gap_samples = 0;

            for (int j = i; j < i + big_gap; j++)//Складываем big_gap значений   
            {
                sum_gap_samples += samples[j];
            }

            mid_gap_sh.push_back(sum_gap_samples / big_gap); //Получаем среднее значение за gap значений и сохраняем его в массив
            temp = i;
        }

        return mid_gap_sh;
    }

    else
    {
        cout << "Ошибка. Меньший отрезок больше или равен большему отрезку!" << endl;
    }

}

//Функция симметричного экспоненциального сглаживания
vector <float> symmetric(const vector<float>& input, float alpha)
{
    if (input.empty()) return {};

    vector<float> filtered = input; // Копируем исходные данные

    // Прямой проход (слева направо)
    for (size_t i = 1; i < filtered.size(); i++)
    {
        filtered[i] = alpha * input[i] + (1 - alpha) * filtered[i - 1];
    }

    // Обратный проход (справа налево) 
    for (int i = filtered.size() - 2; i >= 0; i--) {
        filtered[i] = alpha * filtered[i] + (1 - alpha) * filtered[i + 1];
    }

    return filtered;
}

//Функция для построения фильтрации сокльзящего среднего
vector <float> single_smooth2(const vector<float>& samples, float sensitivity, int gap)
{
    size_t num_samples = samples.size(); //Число семплов в файле  
    vector <float> sig_sm(num_samples, -200);
    vector <float> temp;
    vector <float> expo; //Вектор, хранящий данные после сглаживания (такая же длина как и у samples) 
    float n = 0;

    expo = symmetric(samples, 0.08);
    //Обрежем сигнал, чтобы делился на small_gap без остатка
    expo.erase(expo.end() - (num_samples % gap), expo.end());

    temp = mid_gap(expo, gap); //Делим сигнал на отрезки по gap семплов и находим среднее значение на этих отрезках

    //Цикл для отрисовки сигнала со средними значениями
    /*Т.к.средних значений меньше, чем значений сигнала, то продлеваем вектор
    средних значений так, чтобы на графике строились средние значения и на каком участке оно бралось*/
    for (int i = 0; i < temp.size(); i++)
    {
        for (int j = i * gap; j < (i + 1) * gap; j++)
        {
            sig_sm[j] = temp[i];
        }
    }

    return sig_sm;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//Функция вычисления общего среднего уровня амплитуды
float calc_general_middle(const vector<float>& samples, size_t num_samples)
{
    double sum_all_samples = 0;
    float general_middle;

    for (int i = 0; i < num_samples; i++)
    {
        sum_all_samples += samples[i];

        if (i == (num_samples - 1))
        {
            general_middle = sum_all_samples / num_samples;

            return general_middle;
        }
    }
}

//Функция для выделения одиночного плавного сигнала

vector <float> single_smooth(const vector<float>& samples, float sensitivity, int gap, vector<float>& sig_sm)
{
    size_t num_samples = samples.size(); //Число семплов в файле  
    //vector <float> sig_sm(num_samples, -200);
    vector <float> temp;
    vector <float> temp2;//Вектор, хранящий средние значения вектора expo там, где они были взяты
    vector <float> expo; //Вектор, хранящий данные после сглаживания (такая же длина как и у samples)
    float n = 0;
    vector <float> onehun(num_samples, -2);
    expo = symmetric(samples, 0.08);

    //Обрежем сигнал, чтобы делился на small_gap без остатка
    expo.erase(expo.end() - (num_samples % gap), expo.end());

    temp = mid_gap(expo, gap); //Делим сигнал на отрезки по gap семплов и находим среднее значение на этих отрезках

    //Цикл для отрисовки сигнала со средними значениями

    temp2 = mid_gap_shift(expo, gap, 10 * gap);

    sig_sm = comparison(samples, temp, temp2, gap, gap * 10, sensitivity);

    return sig_sm;

}

vector <float> single_smooth100(const vector<float>& samples, float sensitivity, int gap, const vector<float>& prev)
{
    size_t num_samples = samples.size(); //Число семплов в файле  
    vector <float> sig_sm(num_samples, -200);
    vector <float> temp;
    vector <float> temp2;//Вектор, хранящий средние значения вектора expo там, где они были взяты
    vector <float> expo; //Вектор, хранящий данные после сглаживания (такая же длина как и у samples)
    float n = 0;
    vector <float> onehun(num_samples, -2);
    expo = symmetric(samples, 0.08);

    //Обрежем сигнал, чтобы делился на small_gap без остатка
    expo.erase(expo.end() - (num_samples % gap), expo.end());

    temp = mid_gap(expo, gap); //Делим сигнал на отрезки по gap семплов и находим среднее значение на этих отрезках

    //Цикл для отрисовки сигнала со средними значениями
    /* Т.к.средних значений меньше, чем значений сигнала, то продлеваем вектор
    средних значений так, чтобы на графике строились средние значения и на каком участке оно бралось*/

    temp2 = mid_gap_shift(expo, gap, 10 * gap);


    onehun = comparison100(samples, prev, temp, temp2, gap, gap * 10, sensitivity);
    for (int i = 0; i < num_samples; i++)
    {
        if (prev[i] == 1)
            //if (onehun[i] == 1)
        {
            sig_sm[i] = 1;
        }
    }
    for (int i = 0; i < num_samples; i++)
    {
        //if (prev[i] == 0 and onehun[i] == 1)
        if (onehun[i] == 1)
        {
            sig_sm[i] = 1;
        }
    }

    return sig_sm;
}

//Функция сравнения среднего уровня амплитуды массива и общего среднего уровня, для определения есть ли в файле и сигналы и шумы или там только сигнал/только шум
int find_general_differences(const vector<float>& mid_val_gap, float general_middle, float level_change, bool sens)
{
    int k = 0;
    size_t num_mid_val = mid_val_gap.size(); //Число частей, на сколько делился весь файл

    if (!sens) //Если количественная чувствительность
    {
        for (int i = 0; i < num_mid_val; i++)
        {
            if (mid_val_gap[i] > general_middle + level_change or mid_val_gap[i] < general_middle - level_change)
            {
                k++;
            }
        }
    }

    else //Если качественная чувствительность
    {
        if (general_middle < 0) // Если общее среднее значение меньше нуля
        {
            for (int i = 0; i < num_mid_val; i++)
            {
                if (mid_val_gap[i] < general_middle + (general_middle * level_change) or mid_val_gap[i] > general_middle - (general_middle * level_change))
                {
                    k++;
                }
            }
        }
        //Если general_middle == 0, то любое изменение от среднего уровня скажет, что тут есть сигнал

        else
        {
            for (int i = 0; i < num_mid_val; i++)
            {
                if (mid_val_gap[i] > general_middle + (general_middle * level_change) or mid_val_gap[i] < general_middle - (general_middle * level_change))
                {
                    k++;
                }
            }
        }
    }

    //Сообщение о находке сигнала
    if (k > 0)
    {
        cout << "В этом файле есть и сигналы и шумы!" << endl;

        return 0;
    }
    //Сообщение об ошибке. Ничего не найдено
    else
    {
        cout << "В этом файле либо только сигналы, либо только шумы. Попробуйте изменить уровень чувствительности!" << endl;

        return 1;
    }
}

vector<float> comparison(const vector<float>& samples, const vector<float>& mid_gap, const vector<float>& mid_gap_sh, int small_gap, int big_gap, float sensitivity)
{
    size_t num_samples = samples.size();//Число семплов в файле  
    vector <float> suspicion(num_samples, -200);//Итоговый вектор
    size_t num_mid_gap_sh = mid_gap_sh.size();
    size_t num_mid_gap = mid_gap.size();
    int shift = 6; //Параметр, показывает на сколько значений смещаемся для проверки по мелко-среднему вектору
    size_t val = big_gap / small_gap + shift;
    int front, fall, check_shift = 0, repeat = 0;
    float max_val = -1000000; // Макс значение мелкого вектора
    //float times = pow(10, (sensitivity / 10)); //В times раз должно быть отличие, чтобы мы считали его значимым 
    int p; // Переменная за повторяющийся
    int w_save = 0;//Переменная нужная для повторного сигнала
    //cout << "num_mid_gap_sh: " << num_mid_gap_sh << endl;
    //cout << "num_mid_gap: " << num_mid_gap << endl;
    int b0;
    int b1 = 0; //Параметр отвечающий за количество повторных сигналов
    int b2 = 0; //Параметр отвечающий за число shift при повторе
    //int b3 = 0; //Параметр считающий сигналы. Переменная сделана глобальной
    float b4 = 0; //Параметр для перехода между сигналами 
    int lenght1 = 0; //Длина сигнала 
    int b5 = 0; //Параметр для поиска начала повторяюшегося сигнала
    int b6 = 0; //Параметр для правильного окончания сигнала
    float b7 = 0; //Параметр для нахождения разности между макс и начальным значением
    for (size_t i = 0; i < num_mid_gap_sh; i++)
    {
        //b1 = 0;
        max_val = -1000000;
        p = 0;
        lenght1 = 0;
        b7 = 0;

        if (val + i < num_mid_gap) //Если есть значения мелкого вектора средних, то мы идём проверять  
        {
            max_val = -1000000;

            //Если есть отличие на уровень чувствительности
            if (mid_gap[val + i] > (mid_gap_sh[i] + sensitivity))// and mid_gap[val + i] < (mid_gap_sh[i] + 4 * sensitivity))
            {
                front = 0;

                //Проверяем, чтобы последовательно возрастали все а значений
                for (int j = 0; j < shift + 1; j++)
                {
                    if (mid_gap[val + i - shift + j] < mid_gap[val + i - shift + j + 1])
                    {
                        front++;
                    }
                }

                //Начался сигнал
                if (front >= shift)
                {
                    b3++;
                    //b1++;
                    check_shift = 0;
                    //Если всё сошлось, то записываем там начало сигнала
                    for (size_t d = (val + i - shift) * small_gap; d < num_samples; d++)
                    {
                        if (d == ((val - shift + i) * small_gap))
                        {
                            cout << "Одиночный сигнал номер " << b3 << " начался со значения: " << mid_gap[val - shift + i] << endl;
                        }
                        fall = 0;
                        //Сначала посмотрим есть ли вообще что проверять
                        if (val + i + check_shift + shift < num_mid_gap)
                        {
                            //Будем искать максимальное значение среди соток на этом подъёме
                            if (max_val < mid_gap[val + i + check_shift])
                            {
                                max_val = mid_gap[val + i + check_shift];
                                b7 = max_val - mid_gap[val + i - shift];
                            }
                            //Первые 5 сотен сигналов проставляем единицы, затем через каждые сто сигналов начнём проверять есть ли там конец сигнала
                            if (d >= (val + i) * small_gap and d % small_gap == 0)
                            {
                                check_shift++;
                                // ищем спад. От макс значения на сигнале ищем где он будет отличаться на уровень чувствительности
                                if (max_val - sensitivity > mid_gap[val + i + check_shift])
                                {
                                    //Смотрим есть ли на них уменьшения лесенкой
                                    for (int j = 0; j < shift + 1; j++)
                                    {
                                        if (mid_gap[val + i + check_shift + j] > mid_gap[val + i + check_shift + j + 1])
                                        {
                                            fall++;
                                        }
                                    }

                                    //то сигнал закончился
                                    if (fall == shift - 2)
                                    {
                                        //Проверка, что если через 15 средних есть среднее, которое ниже хотя бы на 1 дБ, то не заканчивать сигнал, мы встретили локальный спад
                                        if (!(mid_gap[val + i + check_shift] - 1 > mid_gap[val + i + check_shift + 3 * shift]))
                                        {

                                            //Проверяем минимум ли мы нашли
                                            for (int r = 0; r < shift; r++)
                                            {
                                                //0.3 добавлено для того, чтобы минимально значимое изменение было хотя бы 0.3 дБ, иначе будет некорректно выделять многие мелочи
                                                if (mid_gap[val + i + check_shift + r] - 0.3 > mid_gap[val + i + check_shift + r + 1])
                                                {
                                                    b6++;
                                                }
                                            }

                                            if (b6 != 0)
                                            {
                                                b6 += 2; //Сдвиг который должен быть из-за особенностей алгоритма
                                                for (int r = 0; r < b6; r++)
                                                {
                                                    for (int r1 = 0; r1 < small_gap * (r + 1); r1++)
                                                    {
                                                        suspicion[d + r1] = 1;
                                                    }
                                                }
                                            }

                                            fall = 0;
                                            // -1 делаем из-за того, что check_shift уже увеличилось при заходе в новую проверку, но с ним проверка не прошла и значение не записалось. А ещё -1, чтобы понять какое последнее значение по итогу было

                                            b4 = d / small_gap - 1 + b6;
                                            cout << "Одиночный сигнал номер: " << b3 << " закончился, d: " << mid_gap[b4] << endl;
                                            cout << "-----------------------------------------------------" << endl;

                                            lenght1 = b4 - val - i + shift;
                                            b6 = 0;
                                            break;
                                        }

                                        else
                                        {
                                            suspicion[d] = 1;
                                        }
                                    }

                                    else
                                    {
                                        suspicion[d] = 1;
                                    }
                                }

                                else
                                {
                                    suspicion[d] = 1;
                                }
                            }

                            else
                            {
                                suspicion[d] = 1;
                            }
                        }
                    }


                    //Цикл для поиска нескольких повторяющихся сигналов
                    for (int s = 0; s < num_mid_gap; s++)
                    {
                        b1++;
                        max_val = -1000000;

                        //Узнаем повторяющийся сигнал или нет. Цикл нужен для смещения, если у нас не shift (пятый) сигнал будет выделяться
                        for (int t = 0; t < 4 * shift; t++)
                        {
                            //val + i + check_shift - 2 - это значение на котором закончился сигнал (последнее включённое в результирующий массив), будем проверять начиная с пятого по десятый семплы т.к big_gap/small_gap = 10, если всё норм, то можно сделать смещение
                            //Проверка на перепад. Перепад также не должен быть супер сильным т.к. мы ищем не резкие сигналы здесь

                            float l = mid_gap[b4] + sensitivity;
                            float l1 = mid_gap[b4 + shift + t];

                            if (l < l1)
                            {
                                for (int r = 0; r < shift + t; r++)///////////////////////////////////////////////////////////////////////МОМЕНТ КОТОРЫЙ МОЖНО ИЗМЕНИТЬ
                                {
                                    if (mid_gap[b4 + r] < mid_gap[b4 + r + 1])
                                    {
                                        repeat++;
                                    }
                                }

                            }

                            if (repeat >= shift)
                            {
                                //Запоминаем значение t, чтобы знать на сколько смещение
                                b0 = t;

                                //Проверка не начался ли сигнал раньше
                                for (int t1 = 0; t1 < t; t1++)
                                {
                                    for (int r = 0; r < shift; r++)
                                    {
                                        if (mid_gap[b4 + t1 + r] < mid_gap[b4 + t1 + r + 1])
                                        {
                                            b5++;
                                        }
                                    }

                                    if (b5 >= shift - 1)
                                    {
                                        b0 = t1 + 2;
                                        b5 = 0;
                                        break;
                                    }

                                    b5 = 0;
                                }

                                break;
                            }

                            else
                            {
                                repeat = 0;
                            }
                        }

                        if (repeat < shift)
                        {
                            cout << "Повторяющийся сигнал НЕ обнаружен" << endl;
                            //Новый сигнал не обнаружен по близости, делаем обычное смещение
                            i += (lenght1);
                            repeat = 0;
                            b1 = 0;
                            b2 = 0;

                            lenght1 = 0;
                            break;
                        }

                        else
                        {
                            if (b0 == 0)
                            {
                                b0 += 2;
                            }

                            check_shift = 0; //Т.к. нашли новый сигнал, то и переменная сдвига обнуляется
                            b3++; //Считаем номер сигнала
                            repeat = 0;
                            int f1 = b4 + b0;
                            cout << "Повторяющийся сигнал номер: " << b3 << " начался со значения: " << mid_gap[f1] << endl;
                            //Начать выделять этот сигнал если нашли повтор. Делаем - 1 в d, чтобы если сигналы буквально идут друг за другому, они были отделены друг от друга
                            for (size_t d = f1 * small_gap; d < num_samples; d++)
                            {
                                fall = 0;
                                //Сначала посмотрим есть ли вообще что проверять
                                if (b4 + b0 + check_shift + shift < num_mid_gap)
                                {
                                    //Будем искать максимальное значение среди соток на этом подъёме
                                    if (max_val < mid_gap[b4 + b0 + check_shift + shift])
                                    {
                                        max_val = mid_gap[b4 + b0 + check_shift + shift];
                                    }
                                    //Первые 5 сотен сигналов проставляем единицы, затем через каждые сто сигналов начнём проверять есть ли там конец сигнала
                                    if (d >= (b4 + b0 + shift) * small_gap and d % small_gap == 0)
                                    {
                                        check_shift++;
                                        // ищем спад. От макс значения на сигнале ищем где он будет отличаться на уровень чувствительности
                                        float now = mid_gap[b4 + b0 + check_shift + shift];
                                        if (max_val - sensitivity > mid_gap[b4 + b0 + check_shift + shift])
                                        {
                                            //Смотрим есть ли на них уменьшения лесенкой
                                            for (int j = 0; j < shift + 1; j++)
                                            {
                                                if (mid_gap[b4 + b0 + check_shift + shift + j] > mid_gap[b4 + b0 + check_shift + shift + j + 1])
                                                {
                                                    fall++;
                                                }
                                            }

                                            //Если на двух, то сигнал закончился
                                            if (fall <= shift - 2)
                                            {
                                                //Проверка, что если через 15 средних есть среднее, которое ниже хотя бы на 1 дБ, то не заканчивать сигнал, мы встретили локальный фронт
                                                if (!(mid_gap[b4 + b0 + check_shift + shift] - 1 > mid_gap[b4 + b0 + check_shift + 4 * shift]))
                                                {
                                                    fall = 0;
                                                    b2++;

                                                    for (int r = 0; r < shift; r++)
                                                    {
                                                        if (mid_gap[val + i + check_shift + r] > mid_gap[val + i + check_shift + r + 1])
                                                        {
                                                            b6++;
                                                        }
                                                    }

                                                    if (b6 != 0)
                                                    {
                                                        b6 += 2;
                                                        for (int r = 0; r < b6; r++)
                                                        {
                                                            for (int r1 = 0; r1 < small_gap * (r + 1); r1++)
                                                            {
                                                                suspicion[d + r1] = 1;
                                                            }
                                                        }
                                                    }
                                                    lenght1 += d / small_gap + b6 - 1 - b4 - b0;
                                                    b4 = d / small_gap - 1 + b6;
                                                    b6 = 0;
                                                    b0 = 0;
                                                    cout << "Повторяющийся сигнал номер: " << b3 << " закончился значением: " << mid_gap[b4] << endl;
                                                    cout << "________________________________________________________________" << endl;
                                                    break;
                                                }

                                                else
                                                {
                                                    suspicion[d] = 1;
                                                }
                                            }

                                            else
                                            {
                                                suspicion[d] = 1;
                                            }


                                        }

                                        else
                                        {
                                            suspicion[d] = 1;
                                        }
                                    }

                                    else
                                    {
                                        suspicion[d] = 1;
                                    }
                                }
                            }
                        }
                    }
                    //Сделать сдвиг после повторяющегося сигнала
                //УБРАТЬ ЭТОТ BREAK?\, он отладочный!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                //break;
                }
            }
        }

    }

    return suspicion;
}

//Функция для сравнения для большого шага
vector<float> comparison100(const vector<float>& samples, const vector<float>& sig_sm, const vector<float>& mid_gap, const vector<float>& mid_gap_sh, int small_gap, int big_gap, float sensitivity)
{
    size_t num_samples = samples.size();//Число семплов в файле  
    vector <float> suspicion(num_samples, -200);//Итоговый вектор
    size_t num_mid_gap_sh = mid_gap_sh.size();
    size_t num_mid_gap = mid_gap.size();
    int shift = 5; //Параметр, показывает на сколько значений смещаемся для проверки по мелко-среднему вектору
    size_t val = big_gap / small_gap + shift;
    int front, fall, check_shift = 0, repeat = 0;
    float max_val = -1000000; // Макс значение мелкого вектора
    //float times = pow(10, (sensitivity / 10)); //В times раз должно быть отличие, чтобы мы считали его значимым 
    int p; // Переменная за повторяющийся
    int w_save = 0;//Переменная нужная для повторного сигнала
    //cout << "num_mid_gap_sh: " << num_mid_gap_sh << endl;
    //cout << "num_mid_gap: " << num_mid_gap << endl;
    int b0;
    int b1 = 0; //Параметр отвечающий за количество повторных сигналов
    int b2 = 0; //Параметр отвечающий за число shift при повторе
    //int b3 = 0; //Параметр считающий сигналы. Сделана глобальной
    float b4 = 0; //Параметр для перехода между сигналами 
    int lenght1 = 0; //Длина сигнала 
    int b5 = 0; //Параметр для поиска начала повторяюшегося сигнала
    int b6 = 0; //Параметр для правильного окончания сигнала
    float b7 = 0; //Параметр для нахождения разности между макс и начальным значением
    int b8 = 0; //Параметр для разрешения начала сигнала, флаг, что там нет мелкого сигнала
    for (size_t i = 0; i < num_mid_gap_sh; i++)
    {

        max_val = -1000000;
        p = 0;
        lenght1 = 0;
        b7 = 0;

        if (val + i < num_mid_gap) //Если есть значения мелкого вектора средних, то мы идём проверять  
        {
            max_val = -1000000;

            //Если есть отличие на уровень чувствительности
            if (mid_gap[val + i] > (mid_gap_sh[i] + sensitivity))// and mid_gap[val + i] < (mid_gap_sh[i] + 4 * sensitivity))
            {
                front = 0;

                //Проверяем, чтобы последовательно возрастали все а значений
                for (int j = 0; j < shift + 1; j++)
                {
                    if (mid_gap[val + i - shift + j] < mid_gap[val + i - shift + j + 1])
                    {
                        front++;
                    }
                }
                b8 = 0;

                //for (int y1 = 0; y1 < big_gap; y1++) 
                //{
                //    //if (val + i - shift + y1 < num_samples) 
                //    //{
                //        if (sig_sm[(val + i - shift) * small_gap + y1] != -160)   
                //        {
                //            b8++; 
                //        }
                //    //}
                //}
                //Начался сигнал
                if (front >= shift and b8 == 0)
                {


                    //b1++;
                    check_shift = 0;
                    //Если всё сошлось, то записываем там начало сигнала
                    for (size_t d = (val + i - shift) * small_gap; d < num_samples; d++)
                    {
                        if (d == ((val - shift + i) * small_gap))
                        {
                            b8 = 0;

                            for (int y1 = 0; y1 < shift * big_gap; y1++)
                            {
                                if (sig_sm[d + y1] == 1)
                                {
                                    b8++;
                                }
                            }

                            if (b8 != 0)
                            {
                                b8 = 0;
                                break;
                            }
                            b3++;
                            cout << "Одиночный сигнал номер " << b3 << " начался со значения: " << mid_gap[val - shift + i] << endl;
                        }
                        fall = 0;
                        //Сначала посмотрим есть ли вообще что проверять
                        if (val + i + check_shift + shift < num_mid_gap)
                        {
                            //Будем искать максимальное значение среди соток на этом подъёме
                            if (max_val < mid_gap[val + i + check_shift])
                            {
                                max_val = mid_gap[val + i + check_shift];
                                b7 = max_val - mid_gap[val + i - shift];
                            }
                            //Первые 5 сотен сигналов проставляем единицы, затем через каждые сто сигналов начнём проверять есть ли там конец сигнала
                            if (d >= (val + i) * small_gap and d % small_gap == 0)
                            {
                                check_shift++;
                                // ищем спад. От макс значения на сигнале ищем где он будет отличаться на уровень чувствительности
                                if (max_val - sensitivity > mid_gap[val + i + check_shift])
                                {
                                    //Смотрим есть ли на них уменьшения лесенкой
                                    for (int j = 0; j < shift + 1; j++)
                                    {
                                        //- 0.2 для того, чтобы при окончании сигнала минимальные изменения, которые уже фактически можно считать шумом не считались и мы считали изменения в 0.2 дБ минимальным
                                        if (mid_gap[val + i + check_shift + j] - 0.2 > mid_gap[val + i + check_shift + j + 1])
                                        {
                                            fall++;
                                        }
                                    }
                                    //Проверка для больших сигналов, что от уровня максимума мы опустились ниже чем на 50%
                                    if ((b7 * 0.4) < (max_val - mid_gap[val + i + check_shift]))
                                    {
                                        //то сигнал закончился
                                        if (fall <= shift - 2)
                                        {
                                            //Проверка, что если через 15 средних есть среднее, которое ниже хотя бы на 1 дБ, то не заканчивать сигнал, мы встретили локальный фронт
                                            if (!(mid_gap[val + i + check_shift] - 1 > mid_gap[val + i + check_shift + 3 * shift]))
                                            {

                                                //Проверяем минимум ли мы нашли
                                                for (int r = 0; r < shift; r++)
                                                {
                                                    if (mid_gap[val + i + check_shift + r] > mid_gap[val + i + check_shift + r + 1])
                                                    {
                                                        b6++;
                                                    }
                                                }

                                                if (b6 != 0)
                                                {
                                                    b6 += 2; //Сдвиг который должен быть из-за особенностей алгоритма
                                                    for (int r = 0; r < b6; r++)
                                                    {
                                                        for (int r1 = 0; r1 < small_gap * (r + 1); r1++)
                                                        {
                                                            suspicion[d + r1] = 1;
                                                        }
                                                    }
                                                }

                                                fall = 0;
                                                // -1 делаем из-за того, что check_shift уже увеличилось при заходе в новую проверку, но с ним проверка не прошла и значение не записалось. А ещё -1, чтобы понять какое последнее значение по итогу было

                                                b4 = d / small_gap - 1 + b6;
                                                cout << "Одиночный сигнал номер: " << b3 << " закончился: " << mid_gap[b4] << endl;
                                                cout << "-----------------------------------------------------" << endl;

                                                lenght1 = b4 - val - i + shift;
                                                b6 = 0;
                                                break;

                                            }

                                            else
                                            {
                                                suspicion[d] = 1;
                                            }
                                        }

                                        else
                                        {
                                            suspicion[d] = 1;
                                        }
                                    }

                                    else
                                    {
                                        suspicion[d] = 1;
                                    }
                                }

                                else
                                {
                                    suspicion[d] = 1;
                                }
                            }

                            else
                            {
                                suspicion[d] = 1;
                            }
                        }
                    }


                    //Цикл для поиска нескольких повторяющихся сигналов
                    for (int s = 0; s < num_mid_gap; s++)
                    {
                        if (b8 != 0)
                        {

                            break;
                        }
                        b1++;
                        max_val = -1000000;


                        //Узнаем повторяющийся сигнал или нет. Цикл нужен для смещения, если у нас не shift (пятый) сигнал будет выделяться
                        for (int t = 0; t < 4 * shift; t++)
                        {

                            //val + i + check_shift - 2 - это значение на котором закончился сигнал (последнее включённое в результирующий массив), будем проверять начиная с пятого по десятый семплы т.к big_gap/small_gap = 10, если всё норм, то можно сделать смещение
                            //Проверка на перепад. Перепад также не должен быть супер сильным т.к. мы ищем не резкие сигналы здесь

                            float l = mid_gap[b4] + sensitivity;
                            float l1 = mid_gap[b4 + shift + t];

                            if (l < l1)
                            {
                                for (int r = 0; r < shift + t; r++)///////////////////////////////////////////////////////////////////////МОМЕНТ КОТОРЫЙ МОЖНО ИЗМЕНИТЬ
                                {
                                    if (mid_gap[b4 + r] < mid_gap[b4 + r + 1])
                                    {
                                        repeat++;
                                    }
                                }

                            }

                            if (repeat >= shift)
                            {
                                //Запоминаем значение t, чтобы знать на сколько смещение
                                b0 = t;

                                //Проверка не начался ли сигнал раньше
                                for (int t1 = 0; t1 < t; t1++)
                                {
                                    for (int r = 0; r < shift; r++)
                                    {
                                        if (mid_gap[b4 + t1 + r] < mid_gap[b4 + t1 + r + 1])
                                        {
                                            b5++;
                                        }
                                    }

                                    if (b5 >= shift - 1)
                                    {
                                        b0 = t1 + 2;
                                        b5 = 0;
                                        break;
                                    }

                                    b5 = 0;
                                }

                                break;
                            }

                            else
                            {
                                repeat = 0;
                            }
                        }

                        if (repeat < shift)
                        {
                            cout << "Повторяющийся сигнал НЕ обнаружен и b3: " << b3 << endl;
                            //Новый сигнал не обнаружен по близости, делаем обычное смещение
                            i += (lenght1);
                            repeat = 0;
                            b1 = 0;
                            b2 = 0;

                            lenght1 = 0;
                            break;
                        }

                        else
                        {


                            if (b0 == 0)
                            {
                                b0 += 2;
                            }

                            check_shift = 0; //Т.к. нашли новый сигнал, то и переменная сдвига обнуляется
                            b3++; //Считаем номер сигнала
                            repeat = 0;
                            int f1 = b4 + b0;
                            cout << "Повторяющийся сигнал номер: " << b3 << " начался со значения: " << mid_gap[f1] << endl;
                            //Начать выделять этот сигнал если нашли повтор. Делаем - 1 в d, чтобы если сигналы буквально идут друг за другому, они были отделены друг от друга
                            for (size_t d = f1 * small_gap; d < num_samples; d++)
                            {

                                fall = 0;
                                //Сначала посмотрим есть ли вообще что проверять
                                if (b4 + b0 + check_shift + shift < num_mid_gap)
                                {
                                    //Будем искать максимальное значение среди соток на этом подъёме
                                    if (max_val < mid_gap[b4 + b0 + check_shift + shift])
                                    {
                                        max_val = mid_gap[b4 + b0 + check_shift + shift];
                                    }
                                    //Первые 5 сотен сигналов проставляем единицы, затем через каждые сто сигналов начнём проверять есть ли там конец сигнала
                                    if (d >= (b4 + b0 + shift) * small_gap and d % small_gap == 0)
                                    {
                                        check_shift++;
                                        // ищем спад. От макс значения на сигнале ищем где он будет отличаться на уровень чувствительности
                                        float now = mid_gap[b4 + b0 + check_shift + shift];
                                        if (max_val - sensitivity > mid_gap[b4 + b0 + check_shift + shift])
                                        {
                                            //Смотрим есть ли на них уменьшения лесенкой
                                            for (int j = 0; j < shift + 1; j++)
                                            {
                                                if (mid_gap[b4 + b0 + check_shift + shift + j] > mid_gap[b4 + b0 + check_shift + shift + j + 1])
                                                {
                                                    fall++;
                                                }
                                            }
                                            //Проверка для больших сигналов, что от уровня максимума мы опустились ниже чем на 50%
                                            if ((b7 * 0.4) < (max_val - mid_gap[val + i + check_shift]))
                                            {
                                                //Если на двух, то сигнал закончился
                                                if (fall <= shift - 2)
                                                {
                                                    //Проверка, что если через 15 средних есть среднее, которое ниже хотя бы на 1 дБ, то не заканчивать сигнал, мы встретили локальный фронт
                                                    if (!(mid_gap[b4 + b0 + check_shift + shift] - 1 > mid_gap[b4 + b0 + check_shift + 4 * shift]))
                                                    {
                                                        fall = 0;
                                                        b2++;

                                                        for (int r = 0; r < shift; r++)
                                                        {
                                                            if (mid_gap[val + i + check_shift + r] > mid_gap[val + i + check_shift + r + 1])
                                                            {
                                                                b6++;
                                                            }
                                                        }

                                                        if (b6 != 0)
                                                        {
                                                            b6 += 2;
                                                            for (int r = 0; r < b6; r++)
                                                            {
                                                                for (int r1 = 0; r1 < small_gap * (r + 1); r1++)
                                                                {
                                                                    suspicion[d + r1] = 1;
                                                                }
                                                            }
                                                        }
                                                        lenght1 += d / small_gap + b6 - 1 - b4 - b0;
                                                        b4 = d / small_gap - 1 + b6;
                                                        b6 = 0;
                                                        b0 = 0;
                                                        cout << "Повторяющийся сигнал номер: " << b3 << " закончился значением: " << mid_gap[b4] << endl;
                                                        cout << "________________________________________________________________" << endl;
                                                        break;
                                                    }

                                                    else
                                                    {
                                                        suspicion[d] = 1;
                                                    }
                                                }

                                                else
                                                {
                                                    suspicion[d] = 1;
                                                }
                                            }

                                            else
                                            {
                                                suspicion[d] = 1;
                                            }

                                        }

                                        else
                                        {
                                            suspicion[d] = 1;
                                        }
                                    }

                                    else
                                    {
                                        suspicion[d] = 1;
                                    }
                                }
                            }
                        }
                    }
                    //Сделать сдвиг после повторяющегося сигнала
                //УБРАТЬ ЭТОТ BREAK?\, он отладочный!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                //break;


                }
            }
        }
        //    }
        //}
    }

    return suspicion;
}

