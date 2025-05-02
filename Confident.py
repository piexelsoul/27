import os

class WaveScore:
    WAVE3 = 25
    WAVE5 = 20
    WAVEC = 15
    WAVE1 = 10
    CORRECTIVE = 5

def get_wave_score(position):
    wave_scores = {
        "wave3": WaveScore.WAVE3,
        "wave5": WaveScore.WAVE5,
        "wavec": WaveScore.WAVEC,
        "wave1": WaveScore.WAVE1,
        "corrective": WaveScore.CORRECTIVE
    }
    return wave_scores.get(position.lower(), None)

def validate_bb_status(status):
    return status.lower() in ["overbought", "oversold", "neutral"]

def evaluate_confidence(wave_score, trend, macd, bb, vol, bb_touch, vol_at_bb, strat):
    score = wave_score + (20 if trend else 5) + (20 if macd else 0)
    if bb.lower() in ["overbought", "oversold"]:
        score += 15 if bb_touch else 0
        score += 10 if vol_at_bb else 0
    score += 20 if vol else 5

    return {"score": score, "strategy": strat}

def run_interface():
    while True:
        wave = input("Wave position (wave3/wave5/wavec/wave1/corrective): ").strip().lower()
        wave_score = get_wave_score(wave)
        while wave_score is None:
            print("Invalid wave. Try again.")
            wave = input("Wave position (wave3/wave5/wavec/wave1/corrective): ").strip().lower()
            wave_score = get_wave_score(wave)

        trend = input("Is it following the trend? (yes/no): ").strip().lower() == "yes"
        macd = input("MACD divergence? (yes/no): ").strip().lower() == "yes"

        bb = input("BB status (overbought/oversold/neutral): ").strip().lower()
        while not validate_bb_status(bb):
            print("Invalid BB status. Try again.")
            bb = input("BB status (overbought/oversold/neutral): ").strip().lower()

        vol = input("Volume supporting the signal? (yes/no): ").strip().lower() == "yes"
        bb_touch = input("Price touched neutral after BB? (yes/no): ").strip().lower() == "yes"
        vol_at_bb = input("Volume increased at BB touch? (yes/no): ").strip().lower() == "yes"

        strat = input("Strategy name: ").strip()
        if not strat:
            strat = "MACD"
        strat = strat.upper()

        res = evaluate_confidence(wave_score, trend, macd, bb, vol, bb_touch, vol_at_bb, strat)

        while True:
            print(f"Strategy: {res['strategy']}")
            print(f"Confidence Score: {res['score']}%")
            action = input("Press 'r' to run again, 's' to save, or any other key to exit: ").strip().lower()
            if action == 'r':
                break
            elif action == 's':
                try:
                    with open("confidence_result.txt", "w") as f:
                        f.write(f"Strategy: {res['strategy']}\nConfidence Score: {res['score']}%\n")
                    print("Result saved to confidence_result.txt")
                except Exception as e:
                    print(f"Failed to save result to confidence_result.txt: {str(e)}")
            else:
                return

if __name__ == "__main__":
    run_interface()
