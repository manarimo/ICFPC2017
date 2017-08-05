<board>
    <div style="display: flex">
        <svg viewBox={this.viewBox} width="600" height="600" style="background-color: white">
            <line each={this.rivers}
                  riot-x1={this.siteDict[source].x}
                  riot-y1={this.siteDict[source].y}
                  riot-x2={this.siteDict[target].x}
                  riot-y2={this.siteDict[target].y}
                  stroke-width="0.02"
                  stroke={this.colors[0]}
                  stroke-dasharray="0.1, 0.1"/>
            <line each={h, i in this.histories} if={i < this.frame}
                  riot-x1={this.siteDict[h.move.claim.source].x}
                  riot-y1={this.siteDict[h.move.claim.source].y}
                  riot-x2={this.siteDict[h.move.claim.target].x}
                  riot-y2={this.siteDict[h.move.claim.target].y}
                  stroke-width="0.04"
                  stroke={this.colors[h.move.claim.punter + 1]} />
            <circle each={this.sites} riot-cx={x} riot-cy={y} r="0.05" />
            <circle each={id in this.mines} riot-cx={this.siteDict[id].x} riot-cy={this.siteDict[id].y} r="0.1" style="fill: red" />
        </svg>
        <div>
            <div each={score, punter in this.scores}>
                <span style="width: 16px; height: 16px; background-color: {this.colors[parseInt(punter) + 1]}; display: inline-block"></span>
                <span>Player {punter}: {score} pts</span>
            </div>
            <form onsubmit={submit}>
                <input type="text" id="frame" value={this.frame} /> / <span>{this.histories.length}</span>
                <ul>
                    <li>矢印キーかhjklでフレームを操作できるよぉ</li>
                    <li>テキストボックスに数値を入れてEnterでそこまで飛ぶよぉ</li>
                </ul>
            </form>
        </div>
    </div>
    <script>
        this.on('before-mount', () => {
            this.refresh(opts);
        });

        keyPress(e) {
            console.log(e);
            if (e.keyCode === 37 || e.key === 'h' || e.key === 'k') {
                if (this.frame > 0) {
                    --this.frame;
                    this.update();
                }
            } else if (e.keyCode === 39 || e.key === 'l' || e.key === 'j') {
                if (this.frame < this.histories.length) {
                    ++this.frame;
                    this.update();
                }
            }
        }

        submit(e) {
            e.preventDefault();
            this.frame = parseInt(document.getElementById('frame').value);
            this.update();
        }

        refresh(opts) {
            this.sites = opts.state.map.sites;
            this.rivers = opts.state.map.rivers;
            this.mines = opts.state.map.mines;
            this.histories = opts.state.history;
            this.frame = this.histories.length;
            this.minX = Math.min(...this.sites.map((s) => s.x)) - 1;
            this.minY = Math.min(...this.sites.map((s) => s.y)) - 1;
            this.maxX = Math.max(...this.sites.map((s) => s.x)) + 1;
            this.maxY = Math.max(...this.sites.map((s) => s.y)) + 1;
            this.viewBox = `${this.minX} ${this.minY} ${this.maxX - this.minX} ${this.maxY - this.minY}`;
            this.scores = {};
            this.histories.forEach((h, i) => this.scores[h.move.claim.punter] = h.score);

            this.siteDict = {};
            this.sites.forEach((site) => this.siteDict[site.id] = site);
        }

        this.on('mount', () => {
            window.addEventListener('keydown', this.keyPress);
        });

        this.on('update', () => {
            if (this.histories !== opts.state.history) {
                this.refresh(opts);
            }
            this.scores = {};
            for (let i = 0; i < this.frame; ++i) {
                const h = this.histories[i];
                this.scores[h.move.claim.punter] = h.score;
            }
        });

        this.on('unmount', () => {
            window.removeEventListener('keydown', this.keyPress);
        });

        this.colors =
            ["#c0c0c0",
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