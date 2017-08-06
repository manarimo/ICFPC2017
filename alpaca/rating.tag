<rating>
    <table border>
        <tr>
            <th><input type="text" id="filter" placeholder="filter" onkeyup={updateFilter}/></th>
            <th each="{name in this.names}">
                {name}
            </th>
        </tr>
        <tr each="{me in this.names}">
            <th>
                <p>{me}</p>
                <p>Rating: {this.ratingData[me].rating}</p>
                <match-result result="{this.ratingData[me]}" />
            </th>
            <td each="{opp in this.names}" style="{this.calcStyle(me, opp)}">
                <match-result result="{this.ratingData[me].result[opp]}" />
            </td>
        </tr>
    </table>

    <script>
        fetch('/api/rating')
            .then((response) => response.json())
            .then((json) => {
                this.ratingData = json;
                this.names = Object.keys(this.ratingData).sort((a, b) => this.ratingData[b].rating - this.ratingData[a].rating);
                this.update();
            });

        calcStyle(me, opp) {
            const data = this.ratingData[me].result[opp];
            if (me === opp) {
                return 'background-color: black';
            } else if (data.win + data.lose === 0) {
                return 'background-color: lightgrey';
            }
            return '';
        }

        updateFilter(e) {
            this.filterText = e.target.value;
            this.names = Object.keys(this.ratingData).filter((s) => s.includes(this.filterText)).sort((a, b) => this.ratingData[b].rating - this.ratingData[a].rating);
        }
    </script>
</rating>

<match-result>
    <span>
        <p><b>{(this.prob * 100).toFixed(2)}%</b></p>
        <p>{this.data.win} / {this.data.win + this.data.lose}</p>
        <p>({this.data.draw} draws)</p>
    </span>
    <script>
        this.data = opts.result;
        this.prob = this.data.win / (this.data.win + this.data.lose);
    </script>
    <style>
        p {
            margin: 0;
            line-height: 1em;
            white-space: nowrap;
        }
    </style>
</match-result>