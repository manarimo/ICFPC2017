package alpaca

import (
	"database/sql"
	_ "github.com/go-sql-driver/mysql"
	"fmt"
	"sort"
)

type AdlersprungDB struct {
	db *sql.DB
}

type Match struct {
	Id    int `json:"id"`
	Type  string `json:"type"`
	Names []string `json:"names"`
}
type MatchList []Match

func (ms MatchList) Len() int { return len(ms) }
func (ms MatchList) Swap(i, j int) { ms[i], ms[j] = ms[j], ms[i] }
func (ms MatchList) Less(i, j int) bool { return ms[i].Id < ms[j].Id }


func OpenConnection() *AdlersprungDB {
	db, err := sql.Open("mysql", "root:kaban@tcp(35.194.126.173:3306)/adlersprung")
	if err != nil {
		panic(err.Error())
	}
	return &AdlersprungDB{
		db: db,
	}
}

func (db *AdlersprungDB) CloseConnection() {
	db.db.Close()
	db.db = nil
}

func (db *AdlersprungDB) FetchRecentMatchDatas() ([]Match, error) {
	rows, err := db.db.Query("SELECT a.id, a.match_type, b.player_name FROM match_log AS a LEFT JOIN player_match_log AS b ON a.id = b.match_id ORDER BY a.id DESC, b.player_id ASC LIMIT 100")
	if err != nil {
		fmt.Println(err.Error())
		return nil, err
	}
	defer rows.Close()

	matches := make(map[int]Match)
	for rows.Next() {
		var id int
		var matchType string
		var name string
		rows.Scan(&id, &matchType, &name)
		if _, ok := matches[id]; !ok {
			matches[id] = Match{
				Id:    id,
				Type:  matchType,
				Names: make([]string, 0),
			}
		}
		m := matches[id]
		m.Names = append(m.Names, name)
		matches[id] = m
	}

	matchList := make(MatchList, 0)
	for _, match := range matches {
		matchList = append(matchList, match)
	}
	sort.Sort(matchList)
	for i := len(matchList) / 2 - 1; i >= 0; i-- {
		opp := len(matchList) - i - 1
		matchList[i], matchList[opp] = matchList[opp], matchList[i]
	}
	return matchList, nil
}

func (db *AdlersprungDB) FindMatchLog(id int) (string, error) {
	rows, err := db.db.Query("SELECT log FROM match_log WHERE id = ?", id)
	if err != nil {
		fmt.Println(err.Error())
		return "", err
	}
	defer rows.Close()

	if rows.Next() {
		var log string
		rows.Scan(&log)
		return log, nil
	} else {
		return "", fmt.Errorf("Match log of id = %d not found", id)
	}
}
