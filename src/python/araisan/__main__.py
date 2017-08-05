import requests
import json
import sys


SLACK_WEBHOOK_URL = "https://hooks.slack.com/services/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"
SLACK_BOT_ICON_URL = "https://files.slack.com/files-pri/T6GV5GB71-F6J4Q2QEP/araisan.jpg"


def post_slack(text):
    try:
        requests.post(SLACK_WEBHOOK_URL, data=json.dumps({
            'text': text,
            'username': 'Arai-san',
            'icon_url': SLACK_BOT_ICON_URL,
        }))
    except:
        print("failed to post to slack. no internet connection?")


if __name__ == '__main__':
    if len(sys.argv) < 2:
        text = sys.stdin.read()
    else:
        text = sys.argv[1]
    post_slack(text)
