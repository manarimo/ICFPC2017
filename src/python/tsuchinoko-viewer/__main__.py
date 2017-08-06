from argparse import ArgumentParser
import json
import sys

def get_rank(arr):
    sorted_arr = sorted(arr, reverse=True)
    rank_map = {}
    for i, val in enumerate(sorted_arr):
        if val not in rank_map:
            rank_map[val] = i + 1
    return [rank_map[val] for val in arr]

def add_meta_data(objs):
    # get stats
    for obj in objs:
        for perf in obj["performances"]:
            perf["highest"] = max(perf["scores"])
            perf["average"] = sum(perf["scores"]) / len(perf["scores"])

    for i in range(len(objs[0]["performances"])):
        scores = list(map((lambda obj: obj["performances"][i]["total"]), objs))
        score_max = max(scores)
        score_min = min(scores)
        ranks = get_rank(scores)
        for j, obj in enumerate(objs):
            obj["performances"][i]["rank"] = ranks[j]
            if score_max == score_min:
                obj["performances"][i]["ratio"] = 1
            else:
                obj["performances"][i]["ratio"] = (scores[j] - score_min) / (score_max - score_min)
    return objs

def print_table(objs, headers):
    print("<html><head><style>")
    print_style()
    print("</style></head><body>")
    print("<table border><thead>")
    print_header(headers)
    print("</thead><tbody>")
    for obj in objs:
        print_row(obj)
    print("</tbody></table>")
    print("</body></html>")

def print_header(headers):
    print("<tr>")
    print("<th></th>")
    for column in headers:
        print("<th>{}</th>".format(column))
    print("</tr></thead>")

def print_row(obj):
    print("<tr>")
    print("<th>{}<br/><small>({})</small><br/>".format(obj["ai"], obj["ai_commit"][:7]))
    ranks = list(map(lambda perf: perf["rank"], obj["performances"]))
    print("<small>Ave Rank: {:.2f}</small>".format(sum(ranks) / len(ranks)))
    print("</th>")
    for perf in obj["performances"]:
        print("<td bgcolor={}><center>".format(calculate_color(perf["ratio"])))
        print("rank: {}<br/>".format(perf["rank"]))
        print("<b>{}</b><br/><small>max: {}</small><br/>".format(perf["average"], perf["highest"]))
        print("</center></td>")
    print("</tr>")

def print_style():
    print('table {font-size: 12px; word-wrap:break-word; border-collapse: collapse;}')
    print('table, th, tr, td {border: solid black 1px;}')
    print('th, td {min-width: 90px; max-width: 100px;}')

def calculate_color(ratio):
    red = [222, 102, 65]
    yellow = [242, 229, 92]
    green = [57, 168, 105]
    color = []
    for i in range(3):
        if ratio < 0.5:
            left = red
            right = yellow
            ratio *= 2
        else:
            left = yellow
            right = green
            ratio = (ratio - 0.5) * 2
        color.append(format(int(right[i] * ratio + left[i] * (1 - ratio)), "02X"))
    return "#{}".format("".join(color))

def main():
    parser = ArgumentParser()
    parser.add_argument("--files", type=str, nargs="+", help="Json files")
    args = parser.parse_args()

    version = -1
    
    objs = []
    for file in args.files:
        f = open(file, "r")
        obj = json.loads(f.read())
        f.close()
        if version == -1:
            version = obj["version"]
        if version != obj["version"]:
            print("Use reports with the same version", file = sys.stderr)
            sys.exit()
        objs.append(obj)
    headers = [perf["name"] for perf in objs[0]["performances"]]
    models = add_meta_data(objs)
    print_table(models, headers)

if __name__ == '__main__':
    main()
