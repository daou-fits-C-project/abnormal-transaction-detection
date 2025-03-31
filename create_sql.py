import yfinance as yf
import pandas as pd

ticker = '023590.KQ'

stock = yf.Ticker(ticker)
df = stock.history(period="5y", interval="1d").reset_index()

with open(f"{ticker}.sql", "w", encoding="utf-8") as f:
    for _, row in df.iterrows():
        date_str = row['Date'].strftime('%Y-%m-%d')
        sql = (
            f"INSERT INTO transaction_log (open_price, close_price, high_price, low_price, amount, stock_id, trade_date) VALUES ("
            f"{int(row['Open'])}, {int(row['Close'])}, {int(row['High'])}, {int(row['Low'])}, {int(row['Volume'])}, '{ticker[:-3]}', TO_DATE('{date_str}', 'YYYY-MM-DD'));\n"
        )
        f.write(sql)
    f.write(f"commit;\n")
