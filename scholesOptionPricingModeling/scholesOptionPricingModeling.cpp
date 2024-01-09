
#include <boost/random/normal_distribution.hpp>
#include <boost/math/distributions/normal.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <map>
#include <tuple>

using std::cout, std::endl, std::string, std::vector, std::map, std::tuple;

class Option {
private:

    double strike;
    double experation;
    double price;
    double currentPrice;
    double contracts;
    double pAndl;
    double underlyingPrice;
    double theoreticalCurrentPrice;
    double impliedVolatility;
    string type;
    string tickerSymbol;
    map<string, double> Option_Shareable{};

public:

    map<string, double>* ac_Option_Shareable = &Option_Shareable;

    static double find_d1(double S_0, double X, double T, double r, double sigma) {
        auto step_one = log(S_0 / X);
		auto step_two = (r + ((pow(sigma, 2) / 2))) * T;
		auto step_three = sigma * sqrt(T);
		auto step_four = step_one + step_two;
		auto step_five = step_four / step_three;
		return step_five;
    }

    static double find_d2(double S_0, double X, double T, double r, double sigma, double d1) {
		auto step_one = d1 - sigma * sqrt(T);
        return step_one;
    }

    static double callScholesOptionPriceModel(double S_0, double X, double T, double r, double sigma) {
		T = T / 252.0;

		double d1 = find_d1(S_0, X, T, r, sigma);
		double d2 = find_d2(S_0, X, T, r, sigma, d1);

		double Nd1 = boost::math::cdf(boost::math::normal(), d1);
		double Nd2 = boost::math::cdf(boost::math::normal(), d2);
		long double single_call_price = S_0 * Nd1 - (X * exp(-r * T) * Nd2);

		return single_call_price;
	}

    static double putScholesOptionPriceModel(double S_0, double X, double T, double r, double sigma) { 
        T = T / 252.0;

        double d1 = find_d1(S_0, X, T, r, sigma);
        double d2 = find_d2(S_0, X, T, r, sigma, d1);

        double Nd1 = boost::math::cdf(boost::math::normal(), -d1);
        double Nd2 = boost::math::cdf(boost::math::normal(), -d2);
        long double single_put_price = (X * exp(-r * T) * Nd2) - S_0 * Nd1;

        return single_put_price;
    }

    double findCurrentpAndl() {
            cout << "Contracts: " << contracts << endl;
            return (currentPrice - price) * (contracts * 100);
    }

    Option() {
        Option_Shareable["Strike"] = 0;
        Option_Shareable["Experation"] = 0;
        Option_Shareable["Price"] = 0;
        strike = 0;
        experation = 0;
        price = 0;
        type = "";
        currentPrice = 0;
        tickerSymbol = "";
        contracts = 0;
        pAndl = 0;
    }

    Option(double strike, double experation, double price, string type, double currentPrice, string tickerSymbol, double contracts, double underlyingPrice, double impliedVolatility) {
        Option_Shareable["Strike"] = strike;
        Option_Shareable["Experation"] = experation;
        Option_Shareable["Price"] = price;
        this->strike = strike;
        this->experation = experation;
        this->price = price;
        this->type = type;
        this->currentPrice = currentPrice;
        this->tickerSymbol = tickerSymbol;
        this->contracts = contracts;
        this->pAndl = findCurrentpAndl();
        this->underlyingPrice = underlyingPrice;
        this->impliedVolatility = impliedVolatility;
    }

    ~Option() {
        cout << "Option Object Destroyed" << endl;
    }

    void shareInfo() const {
        cout << "----------" << endl;
        cout << "Strike: " << strike << endl;
        cout << "Experation: " << experation << endl;
        cout << "Price: " << price << endl;
        cout << "Type: " << type << endl;
        cout << "Current Price: " << currentPrice << endl;
        cout << "Ticker Symbol: " << tickerSymbol << endl;
        cout << "Amount Payed: " << price * (contracts * 100) << endl;
        cout << "----------" << endl;
    }

    void setStrike(double strike) {
        Option_Shareable["Strike"] = strike;
        this->strike = strike;
    }

    void setExperation(double experation) {
        Option_Shareable["Experation"] = experation;
        this->experation = experation;
    }

    void setPrice(double price) {
        Option_Shareable["Price"] = price;
        this->price = price;
    }

    void setType(string type) {
        this->type = type;
    }

    void setCurrentPrice(double currentPrice) {
		this->currentPrice = currentPrice;
	}

    void setTickerSymbol(string tickerSymbol) {
		this->tickerSymbol = tickerSymbol;
	}

    double getpAndl() {
		return pAndl;
	}

    double calculatePercentageChange(double currentPrice, double price) {
		return ((currentPrice - price) / price) * 100;
	}

    map<string, vector<double>> findNotables() {
        cout << "----------" << endl;
        int defaultIncrement = 1;
        map<string, vector<double>> notableMap{};
        vector<double> notableVector{};
        if (pAndl < 0) {
            const double realBreakEven = price;
            if (type == "Call") {
                double theoreticalBreakEven = callScholesOptionPriceModel(underlyingPrice, strike, experation, 1, impliedVolatility);
                const double theoreticalUnderlyingBreakEvenPrice = underlyingPrice; 
                double targetUnderlyingPrice = underlyingPrice;
                if (theoreticalBreakEven > realBreakEven) {
                    cout << "-----" << endl;
                    cout << "Options Priced Irrationally, Breakeven Cannot Be Determined" << endl;
                    cout << "Theporetical Breakeven: " << theoreticalBreakEven << endl;
                    cout << "Price Paid: " << price << endl;
                    cout << "-----" << endl;
                    return notableMap;
                }
                while (true) {
                    if (theoreticalBreakEven == realBreakEven || theoreticalBreakEven > realBreakEven) {
                        cout << "-----" << endl;
                        cout << "Required Upward Movement: " << calculatePercentageChange(targetUnderlyingPrice, underlyingPrice) << endl;
                        cout << "From Price: " << underlyingPrice << endl;
                        cout << "To Price: " << targetUnderlyingPrice << endl;
                        cout << "-----" << endl;
                        notableVector.push_back(calculatePercentageChange(targetUnderlyingPrice, underlyingPrice));
                        notableMap["Required Movement"] = notableVector;
                        break;
                    }
                    else
                    {
                        targetUnderlyingPrice += 0.01;
						const double newTheoreticalOptionPrice = callScholesOptionPriceModel(targetUnderlyingPrice, strike, experation, 1, impliedVolatility);
                        theoreticalBreakEven = newTheoreticalOptionPrice;
                        cout << "Current Theoretical Option Price : " << newTheoreticalOptionPrice << endl;
                        cout << "Price Paid : " << price << endl;
                    }
                }
                return notableMap;
            }
            else if (type == "Put") {
                double theoreticalBreakEven = putScholesOptionPriceModel(underlyingPrice, strike, experation, 1, impliedVolatility);
                const double theoreticalUnderlyingBreakEvenPrice = underlyingPrice;
                double targetUnderlyingPrice = underlyingPrice;
                if (theoreticalBreakEven > realBreakEven) {
                    cout << "-----" << endl;
                    cout << "Options Priced Irrationally, Breakeven Cannot Be Determined" << endl;
                    cout << "Theporetical Breakeven: " << theoreticalBreakEven << endl;
                    cout << "Price Paid: " << price << endl;
                    cout << "-----" << endl;
                    return notableMap;
                }
                while (true) {
                    if (theoreticalBreakEven == realBreakEven || theoreticalBreakEven > realBreakEven) {
                        cout << "-----" << endl;
                        cout << "Required Downward Movement: " << calculatePercentageChange(targetUnderlyingPrice, underlyingPrice) << endl;
                        cout << "From Price: " << underlyingPrice << endl;
                        cout << "To Price: " << targetUnderlyingPrice << endl;
                        cout << "-----" << endl;
                        notableVector.push_back(calculatePercentageChange(targetUnderlyingPrice, underlyingPrice));
                        notableMap["Required Movement"] = notableVector;
                        break;
                    }
                    else
                    {
                        targetUnderlyingPrice -= 0.01;
                        const double newTheoreticalOptionPrice = putScholesOptionPriceModel(targetUnderlyingPrice, strike, experation, 1, impliedVolatility);
                        theoreticalBreakEven = newTheoreticalOptionPrice;
                        cout << "Current Theoretical Option Price : " << newTheoreticalOptionPrice << endl;
                        cout << "Price Paid : " << price << endl;
                    }
                }
                return notableMap;
            }
        }
        else {
            if (type == "Call") {
                double initialUnderlying = underlyingPrice;
                for (int i = 0; i < 5; i++) 
                {
                    const int dollarIncrease = defaultIncrement * (i + 1);
                    const double newUnderlyingPrice = initialUnderlying + dollarIncrease;
                    const double newTheoreticalOptionPrice = callScholesOptionPriceModel(newUnderlyingPrice, strike, experation, 1, impliedVolatility);
                    const double optionPriceIncrease = newTheoreticalOptionPrice - price;
                    const double optionProfit = optionPriceIncrease * (contracts * 100);
                    notableVector.push_back(optionProfit);
                }
                notableMap["Profit"] = notableVector;
                return notableMap;
            }
            else if (type == "Put") {
				double initialUnderlying = underlyingPrice;
                for (int i = 0; i < 5; i++)
                {
					const int dollarIncrease = defaultIncrement * (i + 1);
					const double newUnderlyingPrice = initialUnderlying - dollarIncrease;
					const double newTheoreticalOptionPrice = putScholesOptionPriceModel(newUnderlyingPrice, strike, experation, 1, impliedVolatility);
					const double optionPriceIncrease = newTheoreticalOptionPrice - price;
					const double optionProfit = optionPriceIncrease * (contracts * 100);
                    cout << "Option Profit: " << optionProfit << endl;
					notableVector.push_back(optionProfit);
				}
				notableMap["Profit"] = notableVector;
                return notableMap;
			}
		}
    }

    map<string, int> findNotable(int Increment) {

    }

    map<string, int> findNotable(int Increment, int Times) {
         
	}

};


class Future {
private:
	map<string, int> Future_Shareable{};
};

class Swap {
private:
    map<string, int> Swap_Shareable{};
};

class Leveraged {
private:
    map<string, int> Leveraged_Shareable{};    
};

class hold_Derivatives {
private:
    vector<Leveraged> leveragedPositions {};
    vector<Option> optionPositions {};
    vector<Future> futurePositions {};
    vector<Swap> swapPositions {};

    public:
        hold_Derivatives(vector<Option>& options_Basket) {
            optionPositions.reserve(options_Basket.size());
            for (int i = 0; i < options_Basket.size(); i++) {
				optionPositions.push_back(options_Basket[i]);
			}
        };

        hold_Derivatives(Option& option) {
            optionPositions.reserve(1);
            optionPositions.push_back(option);
        };

        void shareDerivativeInfo() {
            if (optionPositions.size() > 0) {
                cout << "Total Options: " << optionPositions.size() << endl;
                for (int i = 0; i < optionPositions.size(); i++) {
					Option* ac_Option = &optionPositions[i];
                    ac_Option->ac_Option_Shareable->at("Strike");
				}
			}
            else {
				cout << "No Option Derivatives" << endl;
			}
            if (futurePositions.size() > 0) {
                for (int i = 0; i < futurePositions.size(); i++) {
                    cout << "Futures Not Constructed Yet" << endl;
                }
            }
            else {
				cout << "No Futures Derivatives" << endl;
			}
            if (swapPositions.size() > 0) {
                for (int i = 0; i < swapPositions.size(); i++) {
                    cout << "Swaps Not Constructed Yet" << endl;
				}
			}
            else {
                cout << "No Swap Derivatives" << endl;  
            }
            if (leveragedPositions.size() > 0) {
                for (int i = 0; i < leveragedPositions.size(); i++) {
                    cout << "Leveraged Not Constructed Yet" << endl;
                }
            }
            else {
				cout << "No Leveraged Derivatives" << endl;
			}
        }

        vector<Option>* acOptions_Basket = &optionPositions;
};

class hold_Stocks {
	private:
	vector<map<string, int>> stockPositions {};
};

class Account {
private:
    double totalBalance;
    double cashBalance;
    double marketValue;
    vector<hold_Stocks> stockPositions;
    vector<hold_Derivatives> derivativePositions;
};

int main() {

    Option AFRM = Option(19, 9, 0.14, "Put", 0.15, "AFRM", 25, 25.98, 0.9434); // Creating Option Position For Affirm Holdings, Inc. (AFRM)
    map<string, vector<double>> AFRM_notables = AFRM.findNotables(); // Applies Merton Formula To Put Option, The result is Stored in a Map

    return 0;
}


