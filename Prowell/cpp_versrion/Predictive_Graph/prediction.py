import numpy as np
from statsmodels.tsa.arima.model import ARIMA

# 전역 변수로 모델 캐싱
trained_model = None

def arima_predict(data, steps):
    """
    ARIMA 예측 함수
    :param data: 최근 30개의 데이터 (리스트)
    :param steps: 예측할 스텝 수
    :return: 예측 결과 (리스트)
    """
    global trained_model

    data = np.array(data)

    if trained_model is None or len(trained_model.data.endog) != len(data):
        # ARIMA 모델 학습 (처음 또는 데이터 크기 변경 시)
        trained_model = ARIMA(data, order=(5, 1, 3)).fit()
    else:
        # 모델 업데이트 (기존 데이터에 새로운 데이터 추가)
        trained_model = trained_model.append(data, refit=False)

    # 미래 값 예측
    forecast = trained_model.forecast(steps=steps)
    return forecast.tolist()
