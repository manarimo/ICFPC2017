package main

import (
	"os"
	"io"
	"path/filepath"
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

type LogSpec struct {
	Name    string   `json:"name"`
	Players []string `json:"players"`
}

func loadMeta(path string) (LogSpec, error) {
	f, err := os.Open(path)
	if err != nil {
		return LogSpec{}, err
	}

	type Meta struct {
		Names []string `json:"names"`
	}
	dec := json.NewDecoder(f)
	meta := Meta{}
	dec.Decode(&meta)

	start := strings.LastIndex(path, "/") + 1
	end := strings.Index(path, "_")
	return LogSpec {
		Name: path[start:end],
		Players: meta.Names,
	}, nil
}

func listLogsHandler(w http.ResponseWriter, r *http.Request) {
	files, err := filepath.Glob(getLogDir() + "/*_meta.json")
	if err != nil {
		w.WriteHeader(404)
		return
	}

	logs := make([]LogSpec, 0)
	for _, f := range files {
		if strings.HasSuffix(f, "meta.json") {
			meta, err := loadMeta(f)
			if err == nil {
				logs = append(logs, meta)
			}
		}
	}

	w.Header().Set("Content-Type", "application/json")
	enc := json.NewEncoder(w)
	enc.Encode(logs)
}

func fetchLogHandler(w http.ResponseWriter, r *http.Request) {
	parts := strings.SplitN(r.URL.Path, "/", 4)
	fmt.Printf("%v", parts)
	logFile := getLogDir() + "/" + parts[len(parts)-1] + ".json"
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
