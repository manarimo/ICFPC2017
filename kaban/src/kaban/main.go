package main

import (
	"database/sql"
	"path/filepath"
	"os"
	"strings"
	"encoding/json"
	"io/ioutil"
	"fmt"
	_ "github.com/go-sql-driver/mysql"
)

type Score struct {
	Score     int `json:"score"`
	RankScore int `json:"rank_score"`
	PunterId  int `json:"punter"`
}

type MetaData struct {
	MatchType string `json:"match_type"`
	Scores    []Score `json:"scores"`
	Names     []string `json:"names"`
}

type Match struct {
	metadata MetaData
	log      string
}

func readMeta(file string) MetaData {
	f, err := os.Open(file)
	if err != nil {
		panic(err.Error())
	}
	defer f.Close()

	dec := json.NewDecoder(f)
	spec := MetaData{}
	dec.Decode(&spec)

	return spec
}

func readLog(file string) string {
	f, err := os.Open(file)
	if err != nil {
		panic(err.Error())
	}
	defer f.Close()

	if res, err := ioutil.ReadAll(f); err != nil {
		panic(err.Error())
	} else {
		return string(res)
	}
}

func main() {
	db, err := sql.Open("mysql", "root:kaban@tcp(35.194.126.173:3306)/adlersprung")
	if err != nil {
		panic(err.Error())
	}
	defer db.Close()

	files, err := filepath.Glob("logs/*_meta.json")
	if err != nil {
		panic(err.Error())
	}

	fmt.Println("Loading data")
	names := make(map[string]bool)
	matches := make([]Match, 0)
	for _, file := range files {
		meta := readMeta(file)
		for i, name := range meta.Names {
			name = strings.TrimSpace(name)
			names[name] = true
			meta.Names[i] = name
		}

		logFile := strings.Replace(file, "_meta", "", 1)
		log := readLog(logFile)
		matches = append(matches, Match{
			metadata: meta,
			log:      log,
		})
	}
	fmt.Printf("Done %d\n", len(matches))

	fmt.Println("Start inserting player")
	stmt, err := db.Prepare("INSERT IGNORE INTO player VALUES (?, ?)")
	if err != nil {
		panic(err.Error())
	}
	defer stmt.Close()

	for name, _ := range names {
		if _, err := stmt.Exec(name, nil); err != nil {
			fmt.Printf("Error: skipping %s due to %s\n", name, err.Error())
		}
	}
	fmt.Println("Done")

	fmt.Println("Start inserting match")
	stmt2, err := db.Prepare("INSERT INTO match_log (match_type, tag, log) VALUES (?, ?, ?)")
	if err != nil {
		panic(err.Error())
	}
	defer stmt2.Close()
	stmt3, err := db.Prepare("INSERT INTO player_match_log (player_name, match_id, rank, score, player_id) VALUES (?, ?, ?, ?, ?)")
	if err != nil {
		panic(err.Error())
	}
	defer stmt3.Close()

	tag := os.Args[1]
	for i, match := range matches {
		result, err := stmt2.Exec(match.metadata.MatchType, tag, match.log)
		if err != nil {
			fmt.Printf("Error: skipping %d due to %s\n", i, err.Error())
			continue
		}
		matchId, err := result.LastInsertId()
		if err != nil {
			fmt.Printf("Error: failed to get last insert id due to %s\n", err.Error())
			continue
		}
		for _, score := range match.metadata.Scores {
			name := match.metadata.Names[score.PunterId]
			_, err := stmt3.Exec(name, matchId, score.RankScore, score.Score, score.PunterId)
			if err != nil {
				fmt.Printf("Error: %s %d %s\n", name, score.PunterId, err.Error())
				continue
			}
		}
	}
	fmt.Println("Done")

	for _, file := range files {
		os.Remove(file)
		os.Remove(strings.Replace(file, "_meta", "", 1))
	}
}
