# Filename: binance_fetcher.py

import os
import pandas as pd
from binance.client import Client
from datetime import datetime
import pytz
import mplfinance as mpf

class FetchClient:
    def __init__(self, api_key, secret_key):
        self.client = Client(api_key, secret_key)

    def convert_to_timestamp(self, date_str, timezone='UTC'):
        dt = datetime.strptime(date_str, '%Y.%m.%d')
        dt = dt.replace(tzinfo=pytz.timezone(timezone))
        return int(dt.timestamp() * 1000)

    def fetch_coin_data(self, symbol, interval, start_date, end_date):
        start_time = self.convert_to_timestamp(start_date, 'Europe/Berlin')
        end_time = self.convert_to_timestamp(end_date, 'Europe/Berlin')
        print("Fetching data from Binance API...")
        return self.client.get_klines(symbol=symbol, interval=interval, startTime=start_time, endTime=end_time)

def determine_candle_color(row):
    return 'green' if float(row['Close']) > float(row['Open']) else 'red'

def calculate_indicators(df):
    print("Calculating indicators...")
    df['Candle Color'] = df.apply(determine_candle_color, axis=1)
    df['HH'] = ''
    df['LL'] = ''
    
    for i in range(len(df)):
        if i >= 10:
            recent_candles = df.iloc[i-10:i]
            
            red_candles = recent_candles[recent_candles['Candle Color'] == 'red']
            if len(red_candles) >= 2 and (recent_candles['High'].idxmax() == i):
                df.at[i, 'HH'] = 'HH'
            
            green_candles = recent_candles[recent_candles['Candle Color'] == 'green']
            if len(green_candles) >= 2 and (recent_candles['Low'].idxmin() == i):
                df.at[i, 'LL'] = 'LL'
    
    return df

def plot_candlestick_with_indicators(df, coin_name, timeframe):
    print("Preparing data for plotting...")
    df['Open'] = pd.to_numeric(df['Open'])
    df['High'] = pd.to_numeric(df['High'])
    df['Low'] = pd.to_numeric(df['Low'])
    df['Close'] = pd.to_numeric(df['Close'])
    df['Volume'] = pd.to_numeric(df['Volume'])
    
    df.set_index('Open Time', inplace=True)
    
    add_plot = []
    
    hh_points = df[df['HH'] == 'HH']
    ll_points = df[df['LL'] == 'LL']
    
    if not hh_points.empty:
        add_plot.append(mpf.make_addplot(hh_points['High'], type='scatter', markersize=100, marker='^', color='blue', panel=0))
    if not ll_points.empty:
        add_plot.append(mpf.make_addplot(ll_points['Low'], type='scatter', markersize=100, marker='v', color='orange', panel=0))
    
    market_colors = mpf.make_marketcolors(up='green', down='red', inherit=True)
    style = mpf.make_mpf_style(
        marketcolors=market_colors, 
        rc={'xtick.labelsize': 10, 'ytick.labelsize': 10}
    )
    
    title = f'{coin_name} {timeframe}'
    print("Plotting the candlestick chart...")
    mpf.plot(
        df, 
        type='candle', 
        style=style, 
        addplot=add_plot, 
        title=title, 
        ylabel='Price', 
        datetime_format='%H:%M'
    )

API_KEY = os.getenv('BINANCE_API_KEY')
SECRET_KEY = os.getenv('BINANCE_SECRET_KEY')

if not API_KEY or not SECRET_KEY:
    raise ValueError("API key and secret key must be set in environment variables 'BINANCE_API_KEY' and 'BINANCE_SECRET_KEY'")

fetch_client = FetchClient(API_KEY, SECRET_KEY)

print("Setting date range and symbol...")
start_date = '2024.06.07'
end_date = '2024.06.08'
symbol = 'SEIUSDT'
interval = Client.KLINE_INTERVAL_15MINUTE

data = fetch_client.fetch_coin_data(symbol, interval, start_date, end_date)

columns = ['Open Time', 'Open', 'High', 'Low', 'Close', 'Volume', 'Close Time', 'Quote Asset Volume', 'Number of Trades', 'Taker Buy Base Asset Volume', 'Taker Buy Quote Asset Volume', 'Ignore']
df = pd.DataFrame(data, columns=columns)

print("Converting timestamps and data types...")
df['Open Time'] = pd.to_datetime(df['Open Time'], unit='ms')
df['Close Time'] = pd.to_datetime(df['Close Time'], unit='ms')

df['Open'] = pd.to_numeric(df['Open'])
df['High'] = pd.to_numeric(df['High'])
df['Low'] = pd.to_numeric(df['Low'])
df['Close'] = pd.to_numeric(df['Close'])
df['Volume'] = pd.to_numeric(df['Volume'])

df = calculate_indicators(df)

plot_candlestick_with_indicators(df, 'SEI', '15min')
print("Process complete.")