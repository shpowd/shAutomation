import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from statsmodels.tsa.arima.model import ARIMA

# 1. 특정 추세를 가지는 데이터 생성
np.random.seed(42)
time = np.arange(-29, 1, 1)  # -29초부터 0초까지

# 상승 추세 + 주기성 (사인파) + 랜덤 노이즈
trend = time * 10  # 선형 상승 추세
seasonality = np.sin(2 * np.pi * time / 10) * 50  # 주기성 (주기 = 10)
noise = np.random.normal(0, 20, len(time))  # 랜덤 노이즈
y = 500 + trend + seasonality + noise  # 전체 데이터

data = pd.DataFrame({'time': time, 'y': y})
data.set_index('time', inplace=True)

# 2. 데이터 시각화
plt.figure(figsize=(12, 6))
plt.plot(data.index, data['y'], label="Generated Data", marker='o')
plt.title("Generated Data with Trend and Seasonality")
plt.xlabel("Time (seconds)")
plt.ylabel("Y Values")
plt.legend()
plt.grid()
plt.show()

# 3. ARIMA 모델 학습
model = ARIMA(data['y'], order=(10, 1, 2))  # p, d, q 값을 임의로 설정
fitted_model = model.fit()

# 4. 예측
forecast_steps = 30  # 1초부터 30초까지 예측
forecast = fitted_model.forecast(steps=forecast_steps)

# 5. 결과 시각화
forecast_time = np.arange(1, 31, 1)
plt.figure(figsize=(12, 6))
plt.plot(data.index, data['y'], label="Original Data", marker='o')
plt.plot(forecast_time, forecast, label="Forecast", marker='x', color='red')
plt.axvline(x=0, linestyle='--', color='gray', label='Prediction Start')
plt.title("ARIMA Forecast with Trend and Seasonality")
plt.xlabel("Time (seconds)")
plt.ylabel("Y Values")
plt.legend()
plt.grid()
plt.show()

# 6. 예측 결과 출력
print("Forecasted Values:")
print(forecast)
