# Strategy Results

- Total P&L: ₹108021.75
- Number of trades: 243
- Win rate: 52.26%
- Worst single day: 2025-09-08 — ₹-179298.25

## Assumptions and oddities
- The trade is entered at the 09:30 candle open and exited at the 15:15 candle open, as specified in the brief.
- The previous trading day close is used as the comparison baseline against today's 09:30 price.
- Position size is set to 75 units per lot and each round trip costs ₹40.
- Only dates with valid 09:30 and 15:15 values were included; the dataset appears otherwise clean and complete for trading sessions.
- The worst drawdown is concentrated in a few large negative days, which suggests the strategy is highly path-dependent and not robust enough for blind real-money deployment.

## Would I put real money into this?
I would not put real money into this strategy as-is. The total is positive on the synthetic data, but the drawdowns are large and concentrated in a few days. That means the edge looks fragile and not durable enough to trust outside a controlled test environment.
