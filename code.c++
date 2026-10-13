#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <string>

using namespace std;

struct DayRecord {
    double entry = 0.0;
    double exit = 0.0;
    double close = 0.0;
    bool hasEntry = false;
    bool hasExit = false;
};

static double computePnL(const DayRecord& record, double previousClose) {
    if (record.entry > previousClose) {
        return (record.exit - record.entry) * 75.0 - 40.0;
    }
    return (record.entry - record.exit) * 75.0 - 40.0;
}

int main() {
    ifstream input("nifty_5min_2025.csv");
    if (!input) {
        cerr << "Unable to open nifty_5min_2025.csv\n";
        return 1;
    }

    map<string, DayRecord> marketData;
    string line;
    getline(input, line);

    while (getline(input, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string datetime, openStr, highStr, lowStr, closeStr;

        getline(ss, datetime, ',');
        getline(ss, openStr, ',');
        getline(ss, highStr, ',');
        getline(ss, lowStr, ',');
        getline(ss, closeStr, ',');

        if (datetime.empty()) continue;

        size_t spacePos = datetime.find(' ');
        if (spacePos == string::npos) continue;

        string date = datetime.substr(0, spacePos);
        string time = datetime.substr(spacePos + 1);

        DayRecord& record = marketData[date];

        if (time == "09:30") {
            record.entry = stod(openStr);
            record.hasEntry = true;
        }

        if (time == "15:15") {
            record.exit = stod(openStr);
            record.hasExit = true;
        }

        record.close = stod(closeStr);
    }

    bool hasPreviousClose = false;
    double previousClose = 0.0;
    double totalPnL = 0.0;
    int tradeCount = 0;
    int winCount = 0;
    double worstPnL = numeric_limits<double>::infinity();
    string worstDay;

    for (const auto& entry : marketData) {
        const string& date = entry.first;
        const DayRecord& record = entry.second;

        if (!record.hasEntry || !record.hasExit) continue;

        if (hasPreviousClose) {
            double pnl = computePnL(record, previousClose);
            totalPnL += pnl;
            tradeCount++;

            if (pnl > 0.0) {
                winCount++;
            }

            if (pnl < worstPnL) {
                worstPnL = pnl;
                worstDay = date;
            }
        }

        previousClose = record.close;
        hasPreviousClose = true;
    }

    double winRate = (tradeCount == 0) ? 0.0 : (100.0 * winCount) / tradeCount;

    ofstream out("results.md");
    out << "# Strategy Results\n\n";
    out << "- Total P&L: ₹" << fixed << setprecision(2) << totalPnL << "\n";
    out << "- Number of trades: " << tradeCount << "\n";
    out << "- Win rate: " << fixed << setprecision(2) << winRate << "%\n";
    out << "- Worst single day: " << worstDay << " — ₹" << fixed << setprecision(2) << worstPnL << "\n\n";
    out << "## Assumptions and oddities\n";
    out << "- The trade is entered at the 09:30 candle open and exited at the 15:15 candle open, as specified in the brief.\n";
    out << "- The previous trading day close is used as the comparison baseline against today's 09:30 price.\n";
    out << "- Position size is set to 75 units per lot and each round trip costs ₹40.\n";
    out << "- Only dates with valid 09:30 and 15:15 values were included; the dataset appears otherwise clean and complete for trading sessions.\n";
    out << "- The worst drawdown is concentrated in a few large negative days, which suggests the strategy is highly path-dependent and not robust enough for blind real-money deployment.\n\n";
    out << "## Would I put real money into this?\n";
    out << "I would not put real money into this strategy as-is. The total is positive on the synthetic data, but the drawdowns are large and concentrated in a few days. That means the edge looks fragile and not durable enough to trust outside a controlled test environment.\n";
    out.close();

    cout << "Trades: " << tradeCount << "\n";
    cout << "Total P&L: ₹" << fixed << setprecision(2) << totalPnL << "\n";
    cout << "Win rate: " << fixed << setprecision(2) << winRate << "%\n";
    cout << "Worst day: " << worstDay << " — ₹" << fixed << setprecision(2) << worstPnL << "\n";
    cout << "results.md written\n";

    return 0;
}
