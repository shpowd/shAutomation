# import numpy as np
# from statsmodels.tsa.arima.model import ARIMA

# # 전역 변수로 모델 캐싱
# trained_model = None

# def arima_predict(data, steps):
#     global trained_model

#     data = np.array(data)

#     if trained_model is None or len(trained_model.data.endog) != len(data):
#         trained_model = ARIMA(data, order=(5, 2, 2)).fit()
#     else:
#         trained_model = trained_model.append(data, refit=False)

#     forecast = trained_model.forecast(steps=steps)
#     return forecast.tolist()

import numpy as np
from statsmodels.tsa.arima.model import ARIMA

# 각 그래프의 예측 모델을 개별적으로 관리
def arima_predict(data, steps, model_cache={}):
    data = np.array(data)
    data_key = tuple(data)  # 데이터를 키 값으로 사용

    # 모델 캐싱 (각 데이터셋마다 독립적인 모델 저장)
    if data_key not in model_cache or len(model_cache[data_key].data.endog) != len(data):
        model_cache[data_key] = ARIMA(data, order=(5, 2, 2)).fit()
    else:
        model_cache[data_key] = model_cache[data_key].append(data, refit=False)

    forecast = model_cache[data_key].forecast(steps=steps)
    return forecast.tolist()
