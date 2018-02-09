#include<iostream>
#include<fstream>
#include<cstring>
#include<string>
#include <iomanip>
#include<sstream>
#include<map>
#include<vector>
#include<algorithm>
#include<math.h>
using namespace std;

double sigmoid(double temp){
	return 1/(1 + exp(-temp));
}
struct data_type{
	vector<double> data;
	int type;
};

void W_initial(vector<vector<double>> &d,double a,int num_hidenode,int num_tag){//num_w have add 1
	for(int i = 0;i < num_hidenode;i++){
		vector<double> each_w;
		for(int j = 0;j < num_tag;j++){
			each_w.push_back(a);
		} 
		d.push_back(each_w);
	}
	return;
}
//更新权重 直到满足迭代次数； 
void W_change(vector<vector<double>> &hide_w,vector<vector<double>> &out_w,vector<data_type> d,int num_iter,double bound,double learn_long1,double learn_long2){
	vector<double> hide_result;
	for(int i = 0;i < hide_w.size();i++){
		hide_result.push_back(1);
	}
	int num = 0;
	while(1){
		num++;
		cout<<"count:"<<num<<endl;
		int change_g = 0;
		int change_e = 0;
		for(int i = 0;i < d.size();i++){
			for(int hide = 1;hide < hide_w.size();hide++){
				double temp = 0;
				for(int j = 0;j < d[i].data.size();j++){
					temp += d[i].data[j] * hide_w[hide][j];
				}
				hide_result[hide] = sigmoid(temp);
			}
			vector<vector<double>> temp_out_w = out_w;
			double temp = 0;
			for(int j = 0;j < out_w[0].size();j++){
				temp += hide_result[j]*out_w[0][j];
			}
			temp = sigmoid(temp);
			double g = temp * (1 - temp ) * (d[i].type - temp);
			//cout<<"g:"<<g<<endl;
			for(int j = 0;j < out_w[0].size();j++){
				double error = g * hide_result[j];
				if(abs(error) > bound) { out_w[0][j] += learn_long1 * error; change_g++;
				}
			}
			for(int hide = 0;hide < hide_w.size();hide++){
				double e = hide_result[hide]*(1 - hide_result[hide]) * temp_out_w[0][hide] * g;
				for(int j = 0;j < d[i].data.size();j++){
					double error = e * d[i].data[j];
					if(abs(error) > bound)  { hide_w[hide][j] += learn_long2 * error;change_e++;
					}
				}
				
			}
		}
		cout<<"change_g "<<change_g<<endl;
		cout<<"change_e "<<change_e<<endl;
		if((change_g <= 100 && change_e <= 2) || num > 3*change_g) break;
	}
		//learn_long *= 0.995;	
}

void LRdata_initial(vector<data_type> &d,int num_tag,string a){//read file //insert x0 = 1
	ifstream file1;
	file1.open(a);
	string tab = ",";
	char line[1201];
	file1.getline(line,1200);
	while(!file1.eof()){
		file1.getline(line,1200);
		string thisline  = line;
		int found = thisline.find(tab);
		while(found!=-1){
			thisline.replace(found,1," ");
			found = thisline.find(tab);
		}
		stringstream ss(thisline);
		int count = 0;
		double each_num;
		vector<double> data;
		int type;
		data.push_back(1);
		while(ss>>each_num){
			count++;
			if(count <= num_tag)
			data.push_back(each_num);
			else
			type = each_num;
		} 
		data_type node;
		node.data = data;
		node.type = type;
		d.push_back(node);
	}
	file1.close();	
}
double allone = 0;
double allzero = 0;
double onezero = 0;
double zeroone = 0;
void check_result(vector<data_type> d,vector<vector<double>> hide_w,vector<vector<double>> out_w,int &right){
	vector<double> hide_result;
	for(int i = 0;i < hide_w.size();i++){
		hide_result.push_back(0);
	}
	for(int i = 0;i < d.size();i++){
		for(int j = 0;j < hide_w.size();j++){
			double temp = 0;
			for(int haha = 0;haha < hide_w[j].size();haha++){
				temp += hide_w[j][haha] * d[i].data[haha];
			}
			hide_result[j] = sigmoid(temp);
		}
		double temp = 0;
		for(int j = 0;j < out_w[0].size();j++){
			temp += hide_result[j]*out_w[0][j];
		}
		if((temp > 0.5 && d[i].type == 1)||(temp <= 0.5 && d[i].type == 0))
		right++;
	}	
}

int main(){
	vector<data_type> all_data;
	string name = "train1.csv";
	const int num_tag = 58;
	LRdata_initial(all_data,num_tag,name);
	vector<vector<double>> hide_w;
	vector<vector<double>> output_w;
	const int num_out = 1;
	int num_hidenode = sqrt(num_tag + 1) + 1;//sqrt(m + n) + a//阈值 
	W_initial(hide_w,0,num_hidenode + 1,num_tag + 1);
	W_initial(output_w,0,num_out,num_hidenode + 1);
	//const int num_iter = 50000;//97
	//double bound = 0.0000000001;
	//double learn_long = 0.05;
	//const int num_iter = 7000;//96
	//double bound = 0.0000000001;
	//double learn_long = 0.02;
	const int num_iter = 100000;//94
	double bound = 0.00000001;
	double learn_long1 = 2;
	double learn_long2 = 0.1;
	W_change(hide_w,output_w,all_data,num_iter,bound,learn_long1,learn_long2);
	
	name = "test1.csv";
	vector<data_type> test_data;
	LRdata_initial(test_data,num_tag,name);
	int right = 0;
	check_result(test_data,hide_w,output_w,right);
	cout<<right<<endl;
	right = 0;
	check_result(all_data,hide_w,output_w,right);
	cout<<right<<endl;
}
