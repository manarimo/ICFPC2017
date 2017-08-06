<board>
    <div style="display: flex">
        <svg viewBox="0 0 630 630" width="630" height="630" style="background-color: white">
            <line each={this.rivers}
                  riot-x1={this.scaleX(this.siteDict[source].x)}
                  riot-y1={this.scaleY(this.siteDict[source].y)}
                  riot-x2={this.scaleX(this.siteDict[target].x)}
                  riot-y2={this.scaleY(this.siteDict[target].y)}
                  stroke-width="2"
                  stroke={this.colors[0]}
                  stroke-dasharray="10, 10"/>
            <line each={h, i in this.histories} if={i < this.frame && h.move.claim}
                  riot-x1={this.scaleX(this.siteDict[h.move.claim.source].x)}
                  riot-y1={this.scaleY(this.siteDict[h.move.claim.source].y)}
                  riot-x2={this.scaleX(this.siteDict[h.move.claim.target].x)}
                  riot-y2={this.scaleY(this.siteDict[h.move.claim.target].y)}
                  stroke-width="4"
                  stroke={this.colors[h.move.claim.punter + 1]} />
            <line each={h in this.splurges}
                  riot-x1={this.scaleX(this.siteDict[h.source].x)}
                  riot-y1={this.scaleY(this.siteDict[h.source].y)}
                  riot-x2={this.scaleX(this.siteDict[h.target].x)}
                  riot-y2={this.scaleY(this.siteDict[h.target].y)}
                  stroke-width="4"
                  stroke={this.colors[h.punter + 1]} />
            <circle each={this.sites} riot-cx={this.scaleX(x)} riot-cy={this.scaleY(y)} r="5" />
            <circle each={id in this.mines} riot-cx={this.scaleX(this.siteDict[id].x)} riot-cy={this.scaleY(this.siteDict[id].y)} r="10" style="fill: red" />
        </svg>
        <div>
            <div each={score, punter in this.scores}>
                <span style="width: 16px; height: 16px; background-color: {this.colors[parseInt(punter) + 1]}; display: inline-block"></span>
                <span>Player {punter}: {score} pts</span>
            </div>
            <form onsubmit={submit}>
                <input type="text" id="frame" value={this.frame} /> / <span>{this.histories.length}</span>
                <div>{this.currentMove()}</div>
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

        currentMove() {
            if (this.frame === -1) {
                return 'Initial state';
            }
            const obj = this.histories[this.frame - 1].move;
            if (obj.option) {
                return `Player ${obj.option.punter} optionally claims an edge.`;
            } else if (obj.splurge) {
                return `Player ${obj.splurge.punter} splurges.`;
            } else if (obj.claim) {
                return `Player ${obj.claim.punter} claims an edge.`;
            } else if (obj.pass) {
                return `Player ${obj.pass.punter} skips this turn.`;
            }
            return `Unknown move`;
        }

        keyPress(e) {
            console.log(e);
            if (e.target.tagName !== 'BODY') {
                return;
            }
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

        updateSplurges() {
            this.splurges = this.histories.filter((h, i) => h.move.splurge && i <= this.frame)
                .reduce((acc, h) => {
                    let prev = null;
                    h.move.splurge.route.forEach((node, i) => {
                        if (prev !== null) {
                            acc.push({
                                source: prev,
                                target: node,
                                punter: h.move.splurge.punter,
                            });
                        }
                        prev = node;
                    });
                    return acc;
                }, []);
        }

        refresh(opts) {
            this.sites = opts.state.map.sites;
            this.rivers = opts.state.map.rivers;
            this.mines = opts.state.map.mines;
            this.histories = opts.state.history;
            this.frame = this.histories.length;
            this.minX = Math.min(...this.sites.map((s) => s.x));
            this.minY = Math.min(...this.sites.map((s) => s.y));
            this.maxX = Math.max(...this.sites.map((s) => s.x));
            this.maxY = Math.max(...this.sites.map((s) => s.y));
            this.scaleFactor = Math.min(600 / (this.maxX - this.minX), 600 / (this.maxY - this.minY));
            this.scores = {};
            this.histories.forEach((h, i) => {
                const obj = h.move.claim || h.move.splurge || h.move.option;
                if (obj) {
                    this.scores[obj.punter] = h.score;
                }
                if (h.move.option) {
                    h.move.claim = h.move.option;
                }
            });
            this.updateSplurges();

            this.siteDict = {};
            this.sites.forEach((site) => this.siteDict[site.id] = site);
        }

        scaleX(val) {
            return this.scaleFactor * (val - this.minX) + 15;
        }

        scaleY(val) {
            return this.scaleFactor * (val - this.minY) + 15;
        }

        this.on('mount', () => {
            window.addEventListener('keydown', this.keyPress);
        });

        this.on('update', () => {
            if (this.histories !== opts.state.history) {
                this.refresh(opts);
            }
            this.updateSplurges();
            this.scores = {};
            for (let i = 0; i < this.frame; ++i) {
                const h = this.histories[i];
                const obj = h.move.claim || h.move.splurge || h.move.option;
                if (obj) {
                    this.scores[obj.punter] = h.score;
                }
            }
        });

        this.on('unmount', () => {
            window.removeEventListener('keydown', this.keyPress);
        });

        this.colors =
            ["#c0c0c0",
                "#aec7e8",
                "#ff7f0e",
                "#dbdb8d",
                "#2ca02c",
                "#d62728",
                "#ffbb78",
                "#98df8a",
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
                "#17becf",
                "#9edae5"];

    </script>
</board>