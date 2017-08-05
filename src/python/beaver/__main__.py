from pathlib import Path
import json


ROOT_DIR = Path(__file__).absolute().parent.parent.parent.parent
LOG_DIR = Path(ROOT_DIR / "logs")
REPORT_DIR = Path(ROOT_DIR / "alpaca_link")
ALPACA_LINK_TEMPLATE = """\
<!DOCTYPE html>
<html>
<head>
    <title></title>
    <meta charset="utf-8">
    <meta http-equiv="refresh" content="URL=http://alpaca.adlersprung.osak.jp/index.html#{json_name}">
</head>
<body>
<script>
    window.location.href = "http://alpaca.adlersprung.osak.jp/index.html#{json_name}";
</script>
<a href="http://alpaca.adlersprung.osak.jp/index.html#{json_name}">ふわああぁ！いらっしゃぁい！よぉこそぉ↑ジャパリカフェへ～！ (アルパカでビジュアライズする)</a>
</body>
</html>
"""


def process(log_path: Path):
    meta_name = log_path.name.replace(".json", "_meta.json")
    meta_path = Path(LOG_DIR / meta_name)
    metadata = dict()
    with log_path.open() as f:
        log_json = json.load(f)
        if "names" in log_json:
            metadata["names"] = log_json["names"]
    with meta_path.open("w") as f:
        json.dump(metadata, f)
    alpaca_link_path = Path(REPORT_DIR / "index.html")
    if not REPORT_DIR.exists():
        REPORT_DIR.mkdir()
    with alpaca_link_path.open("w") as f:
        f.write(ALPACA_LINK_TEMPLATE.format(json_name=log_path.name))


def main():
    for log_path in LOG_DIR.iterdir():
        if log_path.name.endswith(".json"):
            process(log_path)


if __name__ == '__main__':
    main()