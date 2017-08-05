<visualizer>
    <div style="display: flex">
        <board state={this.state} if={this.state} />
        <div>
            <input type="input" id="filter" onkeyup={updateFilter} />
            <ul>
                <li each={log in this.filtered(this.logs)}>
                    <a href="#{log.name}" onclick={change} data-logname={log.name}>{log.name}</a>
                    <span>({log.players.join(', ')})</span>
                </li>
            </ul>
        </div>
    </div>

    <script>
        this.state = null;
        this.text = '';
        fetch('/api/list_logs')
            .then((response) => response.json())
            .then((json) => {
                this.logs = json.filter((s) => s !== '');
                this.update();
            });

        this.on('mount', () => {
            const name = window.location.hash.substr(1);
            if (name) {
                this.load(name);
            }
        });

        load(name) {
            fetch(`/api/logs/${name}`)
                .then((response) => response.json())
                .then((json) => {
                    this.state = json;
                    this.update();
                });
        }

        change(e) {
            const name = e.target.dataset.logname;
            this.load(name);
        }

        updateFilter(e) {
            this.text = e.target.value;
        }

        filtered() {
            return this.logs.filter((spec) => spec.name.includes(this.text) || spec.players.some((p) => p.includes(this.text)));
        }
    </script>
</visualizer>