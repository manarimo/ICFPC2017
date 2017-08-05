<board>
    <div style="display: flex">
        <svg viewBox={this.viewBox} width="600" height="600" style="background-color: white">
            <circle each={this.sites} riot-cx={x} riot-cy={y} r="0.05" />
            <line each={this.rivers}
                  riot-x1={this.siteDict[source].x}
                  riot-y1={this.siteDict[source].y}
                  riot-x2={this.siteDict[target].x}
                  riot-y2={this.siteDict[target].y}
                  stroke-width="0.03"
                  stroke={this.colors[0]} />
            <line each={this.histories}
                  riot-x1={this.siteDict[move.claim.source].x}
                  riot-y1={this.siteDict[move.claim.source].y}
                  riot-x2={this.siteDict[move.claim.target].x}
                  riot-y2={this.siteDict[move.claim.target].y}
                  stroke-width="0.04"
                  stroke={this.colors[move.claim.punter + 1]} />
            <circle each={id in this.mines} riot-cx={this.siteDict[id].x} riot-cy={this.siteDict[id].y} r="0.1" style="fill: red" />
        </svg>
        <div>
            <div each={score, punter in this.scores}>
                <span style="width: 16px; height: 16px; background-color: {this.colors[parseInt(punter) + 1]}; display: inline-block"></span>
                <span>Player {punter}: {score} pts</span>
            </div>
        </div>
    </div>
    <script>
        this.sites = opts.state.map.sites;
        this.rivers = opts.state.map.rivers;
        this.mines = opts.state.map.mines;
        this.histories = opts.state.history;
        this.minX = Math.min(...this.sites.map((s) => s.x)) - 1;
        this.minY = Math.min(...this.sites.map((s) => s.y)) - 1;
        this.maxX = Math.max(...this.sites.map((s) => s.x)) + 1;
        this.maxY = Math.max(...this.sites.map((s) => s.y)) + 1;
        this.viewBox = `${this.minX} ${this.minY} ${this.maxX - this.minX} ${this.maxY - this.minY}`;
        this.scores = {};
        this.histories.forEach((h) => this.scores[h.move.claim.punter] = h.score);

        this.siteDict = {};
        this.sites.forEach((site) => this.siteDict[site.id] = site);

        this.colors =
            ["#1f77b4",
                "#aec7e8",
                "#ff7f0e",
                "#ffbb78",
                "#2ca02c",
                "#98df8a",
                "#d62728",
                "#ff9896",
                "#9467bd",
                "#c5b0d5",
                "#8c564b",
                "#c49c94",
                "#e377c2",
                "#f7b6d2",
                "#7f7f7f",
                "#c7c7c7",
                "#bcbd22",
                "#dbdb8d",
                "#17becf",
                "#9edae5"];

    </script>
</board>