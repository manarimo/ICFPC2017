from pathlib import Path
import json


ROOT_DIR = Path(__file__).absolute().parent.parent.parent.parent
LOG_DIR = Path(ROOT_DIR / "logs")


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


def main():
    for log_path in LOG_DIR.iterdir():
        if log_path.name.endswith(".json"):
            process(log_path)


if __name__ == '__main__':
    main()