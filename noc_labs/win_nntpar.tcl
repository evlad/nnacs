package provide win_nntpar 1.0

package require Tk
package require universal

# Good for dcontrp and dplantid
set OfflineNNTeacherPar {
    -0 "Offline NN training parameters"
    -1 "Параметры обучения"
    eta "Скорость обучения скрытых нейронов"
    eta_output "Скорость обучения выходных нейронов"
    alpha "Коэффициент инерции обучения (моментум)"
    -2 "Параметры останова"
    finish_on_value "Нижняя граница ошибки на тестовой выборке"
    finish_on_decrease "Нижняя граница изменения ошибки на тестовой выборке"
    finish_on_grow "Предельное количество эпох с ростом тестовой ошибки"
    finish_max_epoch "Предельное количество эпох обучения"
    -3 "Параметры визуализации"
    rtseries_yMax "Верхняя граница оси ошибок"
    rtseries_yMin "Нижняя граница оси ошибок"
}

# Good for dcontrf
set OnlineNNTeacherPar {
    -0 "Online NN training parameters"
    -1 "Параметры обучения"
    nnc_auf "Шаг обновления НС-Р по времени"
    eta "Скорость обучения скрытых нейронов"
    eta_output "Скорость обучения выходных нейронов"
    alpha "Коэффициент инерции обучения (моментум)"
    -2 "Параметры останова"
    skip_growing "Пропустить в начале указанное количество эпох с ростом ошибки"
    finish_decrease "Нижняя граница изменения ошибки управления"
    finish_on_grow "Предельное количество эпох с ростом ошибки управления"
    max_epochs "Предельное количество эпох обучения"
    -3 "Параметры визуализации"
    rtseries_yMax "Верхняя граница оси ошибок"
    rtseries_yMin "Нижняя граница оси ошибок"
}
