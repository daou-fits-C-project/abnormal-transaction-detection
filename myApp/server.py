import pandas as pd
import webbrowser

csv_file_path = "FDS_report.csv"
template_file_path = "template.html"
output_file = "FDS_report.html"

try:
    df = pd.read_csv(csv_file_path, encoding="utf-8")
except UnicodeDecodeError:
    df = pd.read_csv(csv_file_path, encoding="cp949")

df["type"] = df["type"].astype(str)
df["created_at"] = pd.to_datetime(df["created_at"])

today = df["created_at"].dt.date.max()
df_today = df[df["created_at"].dt.date == today]

type_order = ["종목 거래량 이상", "계좌 거래량 이상", "고빈도 주문 이상"]

type_summary = (
    df_today.groupby(["type_name", "description"])
    .size()
    .reset_index(name="건수")
)
type_summary.columns = ["이상 유형", "이상 유형 설명", "건수"]
type_summary["이상 유형"] = pd.Categorical(type_summary["이상 유형"], categories=type_order, ordered=True)
type_summary = type_summary.sort_values(by="이상 유형")

stock_table = (
    df_today[df_today["type_name"] == "종목 거래량 이상"]
    [["stock_id", "created_at"]]
    .drop_duplicates()
    .sort_values(by="created_at")
    .rename(columns={"stock_id": "중지 종목 ID", "created_at": "종목 발생일시"})
)

account_table = (
    df_today[df_today["type_name"].isin(["계좌 거래량 이상", "고빈도 주문 이상"])]
    [["account_id", "created_at"]]
    .drop_duplicates()
    .sort_values(by="created_at")
    .rename(columns={"account_id": "중지 계좌 ID", "created_at": "계좌 발생일시"})
)

type_summary_html = f'<div class="scroll-table">{type_summary.to_html(index=False, escape=False)}</div>'
stock_table_html = f'<div class="scroll-table">{stock_table.to_html(index=False, escape=False)}</div>'
account_table_html = f'<div class="scroll-table">{account_table.to_html(index=False, escape=False)}</div>'

summary_html = f"""
<div class="section">
  <div style="display: flex; justify-content: space-between; align-items: center;">
    <h2 style="margin: 0;">이상 거래 요약</h2>
    <span style="color: #828282; font-size: 16px;">{today}</span>
  </div>

  <h3>이상 유형별 건수 및 설명</h3>
  {type_summary_html}

  <h3>거래 중지된 종목 / 계좌</h3>
  <div class="flex-row">
    <div>
        <h4>종목 중지</h4>
        {stock_table_html}
    </div>
    <div>
        <h4>계좌 중지</h4>
        {account_table_html}
    </div>
  </div>
</div>
"""

rename_columns = {
    "order_id": "주문 ID",
    "account_id": "계좌 ID",
    "stock_id": "종목 ID",
    "price": "가격",
    "amount": "수량",
    "type": "주문 유형",
    "type_name": "이상 유형",
    "description": "이상 유형 설명",
    "created_at": "발생 일시"
}

table_map = {
    "종목 거래량 이상": "table_stock_volume",
    "계좌 거래량 이상": "table_account_volume",
    "고빈도 주문 이상": "table_high_freq",
}

table_html_dict = {}
for type_name, placeholder in table_map.items():
    sub_df = df[df["type_name"] == type_name].sort_values(by="created_at", ascending=False)
    sub_df = sub_df.drop(columns=["description"]).rename(columns=rename_columns)
    table_html = f'<div class="scroll-table">{sub_df.to_html(index=False, escape=False)}</div>'
    table_html_dict[placeholder] = table_html

with open(template_file_path, "r", encoding="utf-8") as f:
    html_template = f.read()

html_template = html_template.replace("{summary}", summary_html)
for key, value in table_html_dict.items():
    html_template = html_template.replace(f"{{{key}}}", value)

with open(output_file, "w", encoding="utf-8") as f:
    f.write(html_template)

webbrowser.open(output_file)
