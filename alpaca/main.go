package main

import (
	"os"
	"io"
	"io/ioutil"
	"net/http"
	"encoding/json"
	"strings"
	"fmt"
)

func staticHandler(w http.ResponseWriter, r *http.Request) {
	f, err := os.Open("./" + r.URL.Path)
	if err != nil {
		w.WriteHeader(404)
		return
	}
	defer f.Close()

	if strings.HasSuffix(r.URL.Path, ".html") {
		w.Header().Set("Content-Type", "text/html")
	} else if strings.HasSuffix(r.URL.Path, ".js") {
		w.Header().Set("Content-Type", "text/javascript")
	} else if strings.HasSuffix(r.URL.Path, ".css") {
		w.Header().Set("Content-Type", "text/css")
	} else if strings.HasSuffix(r.URL.Path, ".tag") {
		w.Header().Set("Content-Type", "riot/tag")
	} else if strings.HasSuffix(r.URL.Path, ".jpg") {
		w.Header().Set("Content-Type", "image/jpg")
	}
	io.Copy(w, f)
}

func getLogDir() string {
	logDir, found := os.LookupEnv("LOG_DIR")
	if found {
		return logDir
	}
	return "/var/local/logs"
}

func listLogsHandler(w http.ResponseWriter, r *http.Request) {
	files, err := ioutil.ReadDir(getLogDir())
	if err != nil {
		w.WriteHeader(404)
		return
	}

	logs := make([]string, len(files))
	for _, f := range files {
		if !strings.HasSuffix(f.Name(), "meta.json") {
			logs = append(logs, f.Name())
		}
	}
	w.Header().Set("Content-Type", "application/json")
	enc := json.NewEncoder(w)
	enc.Encode(logs)
}

func fetchLogHandler(w http.ResponseWriter, r *http.Request) {
	parts := strings.SplitN(r.URL.Path, "/", 4)
	fmt.Printf("%v", parts)
	logFile := getLogDir() + "/" + parts[len(parts) - 1]
	f, err := os.Open(logFile)
	if err != nil {
		w.WriteHeader(404)
		return
	}
	defer f.Close()

	w.Header().Set("Content-Type", "application/json")
	io.Copy(w, f)
}

func main() {
	mux := http.NewServeMux()
	mux.HandleFunc("/", staticHandler)
	mux.HandleFunc("/api/list_logs", listLogsHandler)
	mux.HandleFunc("/api/logs/", fetchLogHandler)
	http.ListenAndServe("localhost:8081", mux)
}
