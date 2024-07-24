'''
Программа осуществляет расчёт углов ориентации исследуемого объекта по его изображению. 
Объектом является блок с 4 инфракрасными светодиодами, расположенные в форме квадрата.
Алгоритм определяет 3 угла: alpha - угол крена, beta - угол рыскания, gamma - угол тангажа.
Alpha рассчитывается через atan по координатам центров 2 стоящих друг напротив друга сетодиода.
Beta и Gamma определяются также через atan, но для правильного расчёта необходимо знать:
 - текущее расстояние между камерой и объектом;
 - фокусное расстояние объектива камеры;
 - размеры матрицы камеры;
 - расстояние между напротив стоящими светодиодами;
 - диаметр светодиода.
Через эти параметры определяются опорные длины между напротив стоящими светодиодами и затем
в зависимости от разности между опорными и текущими длинами рассчитываются углы beta и gamma

'''
import numpy as np
import cv2
import math as m

#Константы, используемые в алгоритме

EXPOSURE = -11.0            #Экспозиция камеры
HSV_LOW = [100, 0, 50]      #Нижняя и верхняя границы цветового диапазона в HSV
HSV_HIGH = [190, 255, 255]  

I_THRESHOLD = 25            #Пороговое значение интенсивности пикселя
Y_SORT_THRESHOLD = 2        #Пороговое значение разницы координат Y между соседними пикселями в секции с сортировкой координат

L_0 = 35                    #Расстояние между стоящими друг напротив друга светодиодами (в мм)
L_DIOD = 5                  #Диаметр одного светодиода (в мм)
MATRIX_X = 5.02             #Размеры матрицы камеры (в мм)
MATRIX_Y = 3.76
FOCAL_LENGTH = 8.5          #Фокусное расстояние объективы камеры (в мм)
DISTANCE = 360              #Расстояние между объективом камеры и измеряемым объектом

FILT_LEN = 11               #Размер фильтра скользящего среднего

#Подготовка настроек камеры и массивов для фильтра скользящего среднего

cap = cv2.VideoCapture(1)
cap.set(cv2.CAP_PROP_EXPOSURE, EXPOSURE)

alpha_arr = [0]*FILT_LEN
beta_arr = [0]*FILT_LEN
gamma_arr = [0]*FILT_LEN

if not cap.isOpened():
    print("Cannot open camera")
    exit()

# Секция покадровой съемки и преобразования цветовой модели кадра из RGB в HSV

while True:

    ret, frame = cap.read()

    if not ret:
        print("Can't receive frame (stream end?). Exiting ...")
        break

    rows,cols,colours = frame.shape

    M = cv2.getRotationMatrix2D(((cols-1)/2.0,(rows-1)/2.0),180,1)
    frame = cv2.warpAffine(frame,M,(cols,rows))

    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    lower_IR = np.array(HSV_LOW)
    upper_IR = np.array(HSV_HIGH) 

    mask = cv2.inRange(hsv, lower_IR, upper_IR)
    mask = cv2.bitwise_and(frame,frame, mask = mask)
    mask = cv2.cvtColor(mask, cv2.COLOR_BGR2GRAY)
    mask_tr = mask.transpose()

# Секция поиска координат всех "нетемных" пикселей 

    XY_all = list()
    
    for i in range(rows-1):
        temp = mask[i]
        for j in range(cols-1):
            if temp[j] >= I_THRESHOLD:
                XY_all.append([j, i])

# Секция сортировки координат по 4 светодиодам

    XY_sort = [list(), list(), list(), list()]
    
    X_SORT_THRESHOLD = (L_DIOD * FOCAL_LENGTH * cols)/((DISTANCE - FOCAL_LENGTH) * MATRIX_Y) #Автоматический расчёт (по формуле тонкой линзы) порогового значения разницы координат X между соседними пикселями

    num = 0
    count_num = 0
    sort_error = 0

    XY_diff = list()    # XY_diff нужен для отладки кода
    
    for i in range(len(XY_all)-2):
        XY_diff.append([XY_all[i+1][0] - XY_all[i][0], XY_all[i+1][1] - XY_all[i][1]])
    
    if len(XY_all):
        XY_sort[0].append(XY_all[0])
        for i in range(len(XY_all)-2):
            if XY_all[i+1][1] - XY_all[i][1] <= Y_SORT_THRESHOLD:
                if abs(XY_all[i+1][0] - XY_all[i][0]) <= X_SORT_THRESHOLD + 10:
                    XY_sort[num].append(XY_all[i+1])
                else:
                    count_num += 1
                    if (count_num % 2) == 1:
                        num += 1
                        if num > 3:
                            sort_error = 1
                            break
                        XY_sort[num].append(XY_all[i+1])
                    else:
                        num -= 1
                        if num < 0:
                            sort_error = 1
                            break
                        XY_sort[num].append(XY_all[i+1])
            else:
                if count_num == 0:
                    count_num = 0
                    num += 1
                    if num > 3 or num < 0:
                        sort_error = 1
                        break
                    XY_sort[num].append(XY_all[i+1])
                else:
                    if (count_num % 2) == 1:
                        num += 1
                        if num > 3 or num < 0:
                            sort_error = 1
                            break
                        XY_sort[num].append(XY_all[i+1])
                    else:
                        num += 2
                        if num >= 4:
                            sort_error = 1
                            break
                        XY_sort[num].append(XY_all[i+1])
                    count_num = 0

# Секция расчёта координат центров 4 светодиодов

        if sort_error == 0:     

            XY_res_0 = np.transpose(np.array(XY_sort[0]))
            XY_res_1 = np.transpose(np.array(XY_sort[1]))
            XY_res_2 = np.transpose(np.array(XY_sort[2]))
            XY_res_3 = np.transpose(np.array(XY_sort[3]))
            
            XY_res = [[0, 0], [0, 0], [0, 0], [0, 0]]
            
            if len(XY_res_0) and len(XY_res_1) and len(XY_res_2) and len(XY_res_3):
                XY_res[0] = [round(sum(XY_res_0[0])/len(XY_res_0[0])), round(sum(XY_res_0[1])/len(XY_res_0[1]))]
                XY_res[1] = [round(sum(XY_res_1[0])/len(XY_res_1[0])), round(sum(XY_res_1[1])/len(XY_res_1[1]))]
                XY_res[2] = [round(sum(XY_res_2[0])/len(XY_res_2[0])), round(sum(XY_res_2[1])/len(XY_res_2[1]))]
                XY_res[3] = [round(sum(XY_res_3[0])/len(XY_res_3[0])), round(sum(XY_res_3[1])/len(XY_res_3[1]))]

                for i in range(len(XY_res)):
                    cv2.circle(frame, (XY_res[i]), 2, (255,0,0), 2)
                    cv2.putText(frame, str(XY_res[i]), XY_res[i], cv2.FONT_HERSHEY_SIMPLEX, 0.4, (255,0,0), 1, cv2.FILLED)
                cv2.line(frame, XY_res[0], XY_res[3], (255,0,0), 1)
                cv2.line(frame, XY_res[1], XY_res[2], (255,0,0), 1)

# Секция тригонометрического расчёта углов alpha, beta и gamma

                alpha_x = XY_res[2][0] - XY_res[1][0]
                alpha_y = XY_res[2][1] - XY_res[1][1]
                if alpha_x != 0:
                    alpha = m.atan(alpha_y/alpha_x)/m.pi*180

                    
                    beta_gamma_x = round(m.sqrt(m.pow((XY_res[2][0] - XY_res[1][0]),2) + m.pow((XY_res[2][1] - XY_res[1][1]), 2)))
                    beta_gamma_y = round(m.sqrt(m.pow((XY_res[3][1] - XY_res[0][1]),2) + m.pow((XY_res[3][0] - XY_res[0][0]), 2)))
                    
                    gamma_y0 = round((L_0 * FOCAL_LENGTH * rows)/((DISTANCE - FOCAL_LENGTH) * MATRIX_Y))
                    beta_x0 = round((L_0 * FOCAL_LENGTH * cols)/((DISTANCE - FOCAL_LENGTH) * MATRIX_X))

                    cv2.putText(frame, 'Lx_0 = ' + str(beta_x0), (330, 380), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0,255,0), 1, cv2.FILLED)
                    cv2.putText(frame, 'Ly_0 = ' + str(gamma_y0), (330, 410), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0,255,0), 1, cv2.FILLED)

                    if beta_gamma_y > gamma_y0:
                        gamma = 0
                    else:
                        gamma = m.atan((gamma_y0 - beta_gamma_y)/(FOCAL_LENGTH*2))/m.pi*180
                    
                    if beta_gamma_x > beta_x0:
                        beta = 0
                    else:
                        beta = m.atan((beta_x0 - beta_gamma_x)/(FOCAL_LENGTH*2))/m.pi*180

# Секция применения фильтра скользящего среднего и вывода значений углов на экран

                    alpha_arr[1:FILT_LEN-1] = alpha_arr[0:FILT_LEN-2]
                    beta_arr[1:FILT_LEN-1] = beta_arr[0:FILT_LEN-2]
                    gamma_arr[1:FILT_LEN-1] = gamma_arr[0:FILT_LEN-2]

                    alpha_arr[0] = round(alpha, 3)
                    beta_arr[0] = round(beta, 3)
                    gamma_arr[0] = round(gamma, 3)

                    alpha_out = sum(alpha_arr)/(len(alpha_arr)-1)
                    beta_out = sum(beta_arr)/(len(beta_arr)-1)
                    gamma_out = sum(gamma_arr)/(len(gamma_arr)-1)

                    alpha_txt = 'Alpha = ' + str(round(alpha_out, 3))
                    beta_txt = 'Beta = ' + str(round(beta_out, 3))
                    gamma_txt = 'Gamma = ' + str(round(gamma_out, 3))

                    Lx_txt = 'Lx = ' + str(beta_gamma_x)
                    Ly_txt = 'Ly = ' + str(beta_gamma_y)
                    
                    cv2.putText(frame, alpha_txt, (30, 410), cv2.FONT_HERSHEY_SIMPLEX, 1, (0,255,0), 2, cv2.FILLED)
                    cv2.putText(frame, beta_txt, (30, 440), cv2.FONT_HERSHEY_SIMPLEX, 1, (0,255,0), 2, cv2.FILLED)
                    cv2.putText(frame, gamma_txt, (30, 470), cv2.FONT_HERSHEY_SIMPLEX, 1, (0,255,0), 2, cv2.FILLED)

                    cv2.putText(frame, Lx_txt, (330, 440), cv2.FONT_HERSHEY_SIMPLEX, 1, (0,255,0), 2, cv2.FILLED)
                    cv2.putText(frame, Ly_txt, (330, 470), cv2.FONT_HERSHEY_SIMPLEX, 1, (0,255,0), 2, cv2.FILLED)
                else:
                    cv2.putText(frame, 'NaN', (50, 430), cv2.FONT_HERSHEY_SIMPLEX, 2, (0,0,255), 2, cv2.LINE_AA)
            else:
                cv2.putText(frame, 'NaN', (50, 430), cv2.FONT_HERSHEY_SIMPLEX, 2, (0,0,255), 2, cv2.LINE_AA)
        else:
            sort_error = 0
            cv2.putText(frame, 'NaN', (50, 430), cv2.FONT_HERSHEY_SIMPLEX, 2, (0,0,255), 2, cv2.LINE_AA)
    else:
        cv2.putText(frame, 'NaN', (50, 430), cv2.FONT_HERSHEY_SIMPLEX, 2, (0,0,255), 2, cv2.LINE_AA)
    cv2.imshow('frame', frame)
    cv2.imshow('mask', mask)
    if cv2.waitKey(1) == ord('q'):
        break   
    
    