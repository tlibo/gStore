/*=============================================================================
# Filename: BGPQuery.cpp
# Author: Linglin Yang
# Mail: linglinyang@stu.pku.edu.cn
=============================================================================*/

#include "BGPQuery.h"

using namespace std;


VarDescriptor::VarDescriptor(unsigned id, VarType var_type, const string &var_name):
	id_(id), var_type_(var_type), var_name_(var_name), selected_(false), degree_(0),
	so_var_var_edge_num(0), so_var_con_edge_num(0) {};


// bool VarDescriptor::get_edge_type(int edge_id) {
// 	return this->edge_type_[edge_id];
// }
//
// int VarDescriptor::get_edge_nei(int edge_id) {
// 	return this->edge_nei_[edge_id];
// }
//
// int VarDescriptor::get_edge_index(int edge_id) {
// 	return this->edge_index_[edge_id];
// }

void VarDescriptor::update_so_var_edge_info(unsigned int edge_nei_id, TYPE_PREDICATE_ID pre_id, char edge_type,
											unsigned int edge_index, bool pre_is_var, bool edge_nei_is_var) {
	if(edge_nei_is_var){
		this->var_edge_nei_.push_back(edge_nei_id);
		this->var_edge_pre_id_.push_back(pre_id);
		this->var_edge_type_.push_back(edge_type);
		this->var_edge_index_.push_back(edge_index);

		if(pre_is_var){
			this->var_edge_pre_type_.push_back(PreType::VarPreType);
		} else{
			this->var_edge_pre_type_.push_back(PreType::ConPreType);
		}
	} else{
		this->con_edge_nei_.push_back(edge_nei_id);
		this->con_edge_pre_id_.push_back(pre_id);
		this->con_edge_type_.push_back(edge_type);
		this->con_edge_index_.push_back(edge_index);

		if(pre_is_var){
			this->con_edge_pre_type_.push_back(PreType::VarPreType);
		} else{
			this->con_edge_pre_type_.push_back(PreType::ConPreType);
		}
	}
}

void VarDescriptor::update_pre_var_edge_info(unsigned int s_id, unsigned int o_id, bool s_is_var, bool o_is_var,
											 unsigned int edge_index) {
	this->s_id_.push_back(s_id);
	this->o_id_.push_back(o_id);

	if(s_is_var){
		this->s_type_.push_back(EntiType::VarEntiType);
	} else{
		this->s_type_.push_back(EntiType::ConEntiType);
	}

	if(o_is_var){
		this->o_type_.push_back(EntiType::VarEntiType);
	} else{
		this->o_type_.push_back(EntiType::ConEntiType);
	}

	this->edge_index_.push_back(edge_index);
}

void VarDescriptor::update_statistics_num() {
	if(this->var_type_ == VarDescriptor::VarType::Entity){
		this->so_var_var_edge_num = this->var_edge_type_.size();
		this->so_var_con_edge_num = this->con_edge_type_.size();

		this->degree_ = this->so_var_var_edge_num + this->so_var_con_edge_num;
	} else{
		this->degree_ = this->edge_index_.size();
	}
}

void VarDescriptor::update_select_status(bool selected) {
	this->selected_ = selected;
}

void VarDescriptor::print(KVstore *kvstore) {

	cout << "var: " << var_name_ << " , id is " << id_ << ", var type: " << (var_type_ == VarType::Entity ? "Entity" : "Predicate") << endl;
	cout << "degree = " << degree_ << endl;

	if(var_type_ == VarType::Entity){
		cout << "so_var_var_edge_num = " << so_var_var_edge_num << endl;
		cout << "so_var_con_edge_num = " << so_var_con_edge_num << endl;

		cout << "var edge type, var edge index, var edge nei, var edge pre, var edge pre type: " << endl;
		for(unsigned i = 0; i < var_edge_type_.size(); ++i){
			cout << var_edge_type_[i] << '\t' << var_edge_index_[i] << '\t' << var_edge_nei_[i] << '\t';
			if(var_edge_pre_type_[i] == PreType::VarPreType){
				cout << var_edge_pre_id_[i] << '\t' << "varpretype" << endl;
			} else{
				cout << kvstore->getPredicateByID(var_edge_pre_id_[i]) << '\t' << "conpretype" << endl;
			}
		}

		cout << " con edge type, con edge index, con edge nei, con edge pre, con edge pre type: " << endl;
		for(unsigned i = 0; i < con_edge_type_.size(); ++i){
			cout << con_edge_type_[i] << '\t' << con_edge_index_[i] << '\t' << kvstore->getStringByID(con_edge_nei_[i]) << '\t';
			if(var_edge_pre_type_[i] == PreType::VarPreType){
				cout << var_edge_pre_id_[i] << '\t' << "varpretype" << endl;
			} else{
				cout << kvstore->getPredicateByID(var_edge_pre_id_[i]) << '\t' << "conpretype" << endl;
			}
		}
	} else{
		cout << "s_type_.size() = " << s_type_.size() << endl;
		for(unsigned i = 0; i < s_type_.size(); ++i){

			if(s_type_[i] == EntiType::ConEntiType){
				cout << "s is con, " << kvstore->getStringByID(s_id_[i]) << "\t";
			} else{
				cout << "s is var, " << s_id_[i] << "\t";
			}

			if(o_type_[i] == EntiType::ConEntiType){
				cout << "o is con, " << kvstore->getStringByID(o_id_[i]) << endl;
			} else{
				cout << "o is var, " << o_id_[i] << endl;
			}
		}
	}
}

BGPQuery::BGPQuery() {
	this->initial();

}


// general_evalutial first new one BGPQuery, than initial it.
// than add Triple one by one.
void BGPQuery::initial() {

	this->total_var_num = 0;
	this->total_so_var_num = 0;
	this->total_pre_var_num = 0;

	this->total_join_var_num = 0;
	this->total_selected_var_num = 0;
}


void BGPQuery::AddTriple(const Triple& _triple)
{
	this->triple_vt.push_back(_triple);
}

unsigned BGPQuery::get_var_id_by_name(const string& var_name) {
	return this->var_item_to_id[var_name];
	// for(unsigned i = 0; i < this->var_vector.size(); ++i){
	// 	if(var_vector[i]->var_name_ == var_name){
	// 		return i;
	// 	}
	// }
	//
	// // not found, return -1
	// return -1;
}

unsigned int BGPQuery::get_var_position_by_name(const string &var_name) {
	return this->var_item_to_position[var_name];
}

void BGPQuery::ScanAllVar() {

	bool not_found;
	unsigned index = 0;
	for(const auto &triple : triple_vt){
//		sub
		not_found = (this->item_to_freq.find(triple.subject) == this->item_to_freq.end());
		if(not_found){
			this->item_to_freq[triple.subject] = 1;
			if(triple.subject.at(0) == '?') {
				auto new_sub_var = make_shared<VarDescriptor>(index, VarDescriptor::VarType::Entity, triple.subject);
				// todo: whether this should use std::move?
				this->var_vector.push_back(new_sub_var);

				this->var_item_to_position[triple.subject] = index;
				this->var_item_to_id[triple.subject] = index;

				this->id_position_map[index] = index;
				this->position_id_map[index] = index;

				this->so_var_id.push_back(index);
				this->total_so_var_num += 1;
				index += 1;
			}
		} else{
			++(this->item_to_freq[triple.subject]);
		}

//		pre
		not_found = (this->item_to_freq.find(triple.predicate) == this->item_to_freq.end());
		if(not_found){
			this->item_to_freq[triple.predicate] = 1;
			if(triple.predicate.at(0) == '?') {
				auto new_pre_var = make_shared<VarDescriptor>(index, VarDescriptor::VarType::Predicate, triple.predicate);
				this->var_vector.push_back(new_pre_var);

				this->var_item_to_position[triple.predicate] = index;
				this->var_item_to_id[triple.predicate] = index;

				this->id_position_map[index] = index;
				this->position_id_map[index] = index;

				this->pre_var_id.push_back(index);
				this->total_pre_var_num += 1;
				index += 1;
			}

		} else{
			++(this->item_to_freq[triple.predicate]);
		}

//		obj
		not_found = (this->item_to_freq.find(triple.object) == this->item_to_freq.end());
		if(not_found){
			this->item_to_freq[triple.object] = 1;
			if(triple.object.at(0) == '?') {
				auto new_obj_var = make_shared<VarDescriptor>(index, VarDescriptor::VarType::Entity, triple.object);
				this->var_vector.push_back(new_obj_var);

				this->var_item_to_position[triple.object] = index;
				this->var_item_to_id[triple.object] = index;

				this->id_position_map[index] = index;
				this->position_id_map[index] = index;

				this->so_var_id.push_back(index);
				this->total_so_var_num += 1;
				index += 1;
			}

		} else{
			++(this->item_to_freq[triple.object]);
		}
	}

}

void BGPQuery::build_edge_info(KVstore *_kvstore) {

	for(unsigned i = 0; i < this->triple_vt.size(); ++i){
		string s_string = triple_vt[i].subject;
		string p_string = triple_vt[i].predicate;
		string o_string = triple_vt[i].object;

		bool s_is_var = true;
		bool p_is_var = true;
		bool o_is_var = true;

		TYPE_ENTITY_LITERAL_ID s_id;
		TYPE_ENTITY_LITERAL_ID o_id;
		TYPE_PREDICATE_ID p_id;


		// TODO: if not found, return -1, query has no answer
		if(s_string.at(0) != '?'){
			s_id = _kvstore->getIDByString(s_string);
			s_is_var = false;
		} else{
			s_id = this->get_var_id_by_name(s_string);
		}

		if(o_string.at(0) != '?'){
			o_id = _kvstore->getIDByString(o_string);
			o_is_var = false;
		} else{
			o_id = this->get_var_id_by_name(o_string);
		}

		if(p_string.at(0) != '?'){
			p_id = _kvstore->getIDByPredicate(p_string);
			p_is_var = false;
		} else{
			p_id = this->get_var_id_by_name(p_string);
		}

		// deal with s_var
		if(s_is_var)
			this->var_vector[id_position_map[s_id]]->update_so_var_edge_info(o_id,p_id,Util::EDGE_OUT,i,p_is_var,o_is_var);

		// deal with o_var
		if(o_is_var)
			this->var_vector[id_position_map[o_id]]->update_so_var_edge_info(s_id,p_id,Util::EDGE_IN,i,p_is_var,o_is_var);

		// deal with p_var
		if(p_is_var)
			this->var_vector[id_position_map[p_id]]->update_pre_var_edge_info(s_id,o_id,s_is_var,o_is_var,i);

	}
}

void BGPQuery::count_statistics_num() {

	// TODO: need to check whether this = var_so_num + var_pre_num
	this->total_var_num = this->var_vector.size();

	for(unsigned i = 0; i < this->var_vector.size(); ++ i){
		this->var_vector[i]->update_statistics_num();
	}
}

/**
 *
 * @param _kvstore
 * @param _query_var the
 * @return
 */
bool BGPQuery::EncodeBGPQuery(KVstore *_kvstore, const vector<string> &_query_var) {


	// I want this function scan all vars, incluing all pre_var and subject_or_object_var
	this->ScanAllVar();

	this->build_edge_info(_kvstore);

	this->count_statistics_num();



//	I want to see what is _query_var
	for(unsigned i = 0; i < _query_var.size(); ++i){
		cout << "_query_var[" << i << "] = " << _query_var[i] << endl;
	}

	return true;


}


void BGPQuery::ScanAllVarByBigBGPID(BGPQuery *big_bgpquery) {
	bool not_found;
	unsigned index = 0;
	for(const auto &triple : triple_vt){
		//		sub
		not_found = (this->item_to_freq.find(triple.subject) == this->item_to_freq.end());
		if(not_found){
			this->item_to_freq[triple.subject] = 1;
			if(triple.subject.at(0) == '?') {
				auto new_sub_var = make_shared<VarDescriptor>(big_bgpquery->get_var_id_by_name(triple.subject),
															  VarDescriptor::VarType::Entity, triple.subject);
				this->var_vector.push_back(new_sub_var);

				this->var_item_to_position[triple.subject] = index;
				this->var_item_to_id[triple.subject] = big_bgpquery->get_var_id_by_name(triple.subject);

				this->id_position_map[big_bgpquery->get_var_id_by_name(triple.subject)] = index;
				this->position_id_map[index] = big_bgpquery->get_var_id_by_name(triple.subject);

				this->so_var_id.push_back(big_bgpquery->get_var_id_by_name(triple.subject));
				this->total_so_var_num += 1;
				index += 1;
			}
		} else{
			++(this->item_to_freq[triple.subject]);
		}

		//		pre
		not_found = (this->item_to_freq.find(triple.predicate) == this->item_to_freq.end());
		if(not_found){
			this->item_to_freq[triple.predicate] = 1;
			if(triple.predicate.at(0) == '?') {
				auto new_pre_var = make_shared<VarDescriptor>(big_bgpquery->get_var_id_by_name(triple.predicate),
															  VarDescriptor::VarType::Predicate, triple.predicate);
				this->var_vector.push_back(new_pre_var);

				this->var_item_to_position[triple.predicate] = index;
				this->var_item_to_id[triple.predicate] = big_bgpquery->get_var_id_by_name(triple.predicate);

				this->id_position_map[big_bgpquery->get_var_id_by_name(triple.predicate)] = index;
				this->position_id_map[index] = big_bgpquery->get_var_id_by_name(triple.predicate);

				this->pre_var_id.push_back(big_bgpquery->get_var_id_by_name(triple.predicate));
				this->total_pre_var_num += 1;
				index += 1;
			}

		} else{
			++(this->item_to_freq[triple.predicate]);
		}

		//		obj
		not_found = (this->item_to_freq.find(triple.object) == this->item_to_freq.end());
		if(not_found){
			this->item_to_freq[triple.object] = 1;
			if(triple.object.at(0) == '?') {
				auto new_obj_var = make_shared<VarDescriptor>(big_bgpquery->get_var_id_by_name(triple.object),
															  VarDescriptor::VarType::Entity, triple.object);
				this->var_vector.push_back(new_obj_var);

				this->var_item_to_position[triple.object] = index;
				this->var_item_to_id[triple.object] = big_bgpquery->get_var_id_by_name(triple.object);

				this->id_position_map[big_bgpquery->get_var_id_by_name(triple.object)] = index;
				this->position_id_map[index] = big_bgpquery->get_var_id_by_name(triple.object);

				this->so_var_id.push_back(big_bgpquery->get_var_id_by_name(triple.object));
				this->total_so_var_num += 1;
				index += 1;
			}

		} else{
			++(this->item_to_freq[triple.object]);
		}
	}
}


// this function is invoked after adding all triples of small BGP
bool BGPQuery::EncodeSmallBGPQuery(BGPQuery *big_bgpquery_, KVstore *_kvstore, const vector<string> &_query_var) {

	this->ScanAllVarByBigBGPID(big_bgpquery_);

	this->build_edge_info(_kvstore);

	this->count_statistics_num();

	// todo: return false imply parse error
	return true;

}

unsigned int BGPQuery::get_triple_num() {
	return this->triple_vt.size();
}

unsigned int BGPQuery::get_total_var_num() {
	return this->total_var_num;
}

unsigned int BGPQuery::get_pre_var_num() {
	return this->total_pre_var_num;
}

// bool BGPQuery::get_edge_type(int var_id, int edge_id) {
// 	return var_vector[var_id]->edge_type_[edge_id];
// }
//
// int BGPQuery::get_edge_nei(int var_id, int edge_id) {
// 	return var_vector[var_id]->edge_nei_[edge_id];
// }
//
// int BGPQuery::get_edge_index(int var_id, int edge_id) {
// 	return var_vector[var_id]->edge_index_[edge_id];
// }

/**
 * Print this BGPQuery's info, just for debug, not for user.
 * Use VarDescriptor::print
 * @param kvstore kvstore's pointer
 */
void BGPQuery::print(KVstore *kvstore) {
	cout << "this BGP has " << var_vector.size() << " vars, ie. total_var_num = " << this->total_var_num << endl;

	cout << "total so var num = " << total_so_var_num << ", so_var_id.size() = " << so_var_id.size() << endl;
	for(unsigned i = 0; i < so_var_id.size(); ++ i ){
		cout << "var name: " << var_vector[id_position_map[so_var_id[i]]]->var_name_ << endl;
	}
	cout << "total pre vat num = " << total_pre_var_num << ", pre_var_id.size() = " << pre_var_id.size() << endl;
	for(unsigned i = 0; i < pre_var_id.size(); ++ i){
		cout << "var name: " << var_vector[id_position_map[pre_var_id[i]]]->var_name_ << endl;
	}

	cout << "id position map:" << endl;
	for(auto id_pos : id_position_map){
		cout << id_pos.first << " : " << id_pos.second << endl;
	}
	cout << "position id map:" << endl;
	for(auto pos_id : position_id_map){
		cout << pos_id.first << " : " << pos_id.second << endl;
	}

	cout << "item to freq map:" << endl;
	for(const auto& item_freq : item_to_freq){
		cout << item_freq.first << " : " << item_freq.second << endl;
	}

	cout << "item to position map:" << endl;
	for(const auto& item_pos : var_item_to_position){
		cout << item_pos.first << " : " << item_pos.second << endl;
	}

	cout << "item to id map:" << endl;
	for(const auto& item_id : var_item_to_id){
		cout << item_id.first << " : " << item_id.second << endl;
	}


	cout << endl << "VAR INFO:" << endl;
	for(unsigned i = 0; i < var_vector.size(); ++ i){
		var_vector[i]->print(kvstore);
	}
}