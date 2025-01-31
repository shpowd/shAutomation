import numpy as np
from statsmodels.tsa.arima.model import ARIMA

# 전역 변수로 모델 캐싱
trained_model = None

def arima_predict(data, steps):
    global trained_model

    data = np.array(data)

    if trained_model is None or len(trained_model.data.endog) != len(data):
        trained_model = ARIMA(data, order=(5, 1, 3)).fit()
    else:
        trained_model = trained_model.append(data, refit=False)

    forecast = trained_model.forecast(steps=steps)
    return forecast.tolist()
