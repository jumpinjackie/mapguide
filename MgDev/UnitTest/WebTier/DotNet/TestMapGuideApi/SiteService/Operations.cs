using OSGeo.MapGuide.Test.Common;
using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Operations
{
    public interface IMapGuideSession
    {
        string SessionID { get; set; }
    }

    public class CreateSession : SiteServiceOperationExecutor<CreateSession>
    {
        private IMapGuideSession _session;

        public CreateSession(MgSite site, string unitTestVm, IMapGuideSession session)
            : base(site, unitTestVm)
        {
            _session = session;
        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var user = new MgUserInformation();
                user.SetMgUsernamePassword("Administrator", "admin");
                user.SetLocale("en");

                var site = new MgSite();
                site.Open(user);

                string session = site.CreateSession();
                _session.SessionID = session;
                site.Close();

                return new TestResult(session, "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class DestroySession : SiteServiceOperationExecutor<DestroySession>
    {
        public DestroySession(MgSite site, string unitTestVm)
            : base(site, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                //This is what PHP one is giving us
                return new TestResult("Not Implemented Yet", "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class GetUserForSession : SiteServiceOperationExecutor<GetUserForSession>
    {
        private IMapGuideSession _session;

        public GetUserForSession(MgSite site, string unitTestVm, IMapGuideSession session)
            : base(site, unitTestVm)
        {
            _session = session;
        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var site = new MgSite();
                var user = new MgUserInformation();
                user.SetMgSessionId(_session.SessionID ?? "");
                site.Open(user);
                var userId = site.GetUserForSession();
                site.Close();
                return new TestResult(userId, "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    /*
    public class GetSiteServerAddress : SiteServiceOperationExecutor<GetSiteServerAddress>
    {
        public GetSiteServerAddress(MgSite site, string unitTestVm)
            : base(site, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var result = _site.GetCurrentSiteAddress();
                return new TestResult();
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }
     */

    public class EnumerateUsers : SiteServiceOperationExecutor<EnumerateUsers>
    {
        public EnumerateUsers(MgSite site, string unitTestVm)
            : base(site, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "GROUP", param);
                _unitTestVm.ReadParameterValue(paramSetId, "ROLE", param);
                _unitTestVm.ReadParameterValue(paramSetId, "INCLUDEGROUPS", param);

                MgByteReader reader = null;
                if (param["ROLE"] != null)
                {
                    reader = _site.EnumerateUsers(param["GROUP"] ?? "", param["ROLE"] ?? "", (param["INCLUDEGROUPS"] == "1"));
                }
                else
                {
                    reader = _site.EnumerateUsers(param["GROUP"] ?? "");
                }
                return TestResult.FromByteReader(reader);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class AddUser : SiteServiceOperationExecutor<AddUser>
    {
        public AddUser(MgSite site, string unitTestVm)
            : base(site, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "USERID", param);
                _unitTestVm.ReadParameterValue(paramSetId, "USERNAME", param);
                _unitTestVm.ReadParameterValue(paramSetId, "PASSWORD", param);
                _unitTestVm.ReadParameterValue(paramSetId, "DESCRIPTION", param);

                _site.AddUser(param["USERID"], param["USERNAME"], param["PASSWORD"], param["DESCRIPTION"]);
                return new TestResult();
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class UpdateUser : SiteServiceOperationExecutor<UpdateUser>
    {
        public UpdateUser(MgSite site, string unitTestVm)
            : base(site, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "USERID", param);
                _unitTestVm.ReadParameterValue(paramSetId, "NEWUSERID", param);
                _unitTestVm.ReadParameterValue(paramSetId, "NEWUSERNAME", param);
                _unitTestVm.ReadParameterValue(paramSetId, "NEWPASSWORD", param);
                _unitTestVm.ReadParameterValue(paramSetId, "NEWDESCRIPTION", param);

                _site.UpdateUser(param["USERID"], param["NEWUSERID"], param["NEWUSERNAME"], param["NEWPASSWORD"], param["NEWDESCRIPTION"]);
                return new TestResult();
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class DeleteUsers : SiteServiceOperationExecutor<DeleteUsers>
    {
        public DeleteUsers(MgSite site, string unitTestVm)
            : base(site, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "USERS", param);

                MgStringCollection users = CommonUtility.StringToMgStringCollection(param["USERS"]);

                _site.DeleteUsers(users);
                return new TestResult();
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class GrantRoleMembershipsToUsers : SiteServiceOperationExecutor<GrantRoleMembershipsToUsers>
    {
        public GrantRoleMembershipsToUsers(MgSite site, string unitTestVm)
            : base(site, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "ROLES", param);
                _unitTestVm.ReadParameterValue(paramSetId, "USERS", param);

                MgStringCollection roles = CommonUtility.StringToMgStringCollection(param["ROLES"]);
                MgStringCollection users = CommonUtility.StringToMgStringCollection(param["USERS"]);

                _site.GrantRoleMembershipsToUsers(roles, users);

                return new TestResult();
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class RevokeRoleMembershipsFromUsers : SiteServiceOperationExecutor<RevokeRoleMembershipsFromUsers>
    {
        public RevokeRoleMembershipsFromUsers(MgSite site, string unitTestVm)
            : base(site, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "ROLES", param);
                _unitTestVm.ReadParameterValue(paramSetId, "USERS", param);

                MgStringCollection roles = CommonUtility.StringToMgStringCollection(param["ROLES"]);
                MgStringCollection users = CommonUtility.StringToMgStringCollection(param["USERS"]);

                _site.RevokeRoleMembershipsFromUsers(roles, users);

                return new TestResult();
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class GrantGroupMembershipsToUsers : SiteServiceOperationExecutor<GrantGroupMembershipsToUsers>
    {
        public GrantGroupMembershipsToUsers(MgSite site, string unitTestVm)
            : base(site, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "GROUPS", param);
                _unitTestVm.ReadParameterValue(paramSetId, "USERS", param);

                MgStringCollection groups = CommonUtility.StringToMgStringCollection(param["GROUPS"]);
                MgStringCollection users = CommonUtility.StringToMgStringCollection(param["USERS"]);

                _site.GrantGroupMembershipsToUsers(groups, users);

                return new TestResult();
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class RevokeGroupMembershipsFromUsers : SiteServiceOperationExecutor<RevokeGroupMembershipsFromUsers>
    {
        public RevokeGroupMembershipsFromUsers(MgSite site, string unitTestVm)
            : base(site, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "GROUPS", param);
                _unitTestVm.ReadParameterValue(paramSetId, "USERS", param);

                MgStringCollection groups = CommonUtility.StringToMgStringCollection(param["GROUPS"]);
                MgStringCollection users = CommonUtility.StringToMgStringCollection(param["USERS"]);

                _site.RevokeGroupMembershipsFromUsers(groups, users);

                return new TestResult();
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class EnumerateGroups : SiteServiceOperationExecutor<EnumerateGroups>
    {
        public EnumerateGroups(MgSite site, string unitTestVm)
            : base(site, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "USER", param);
                _unitTestVm.ReadParameterValue(paramSetId, "ROLE", param);

                MgByteReader reader = _site.EnumerateGroups(param["USER"] ?? "", param["ROLE"] ?? "");
                return TestResult.FromByteReader(reader);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class EnumerateGroups2 : SiteServiceOperationExecutor<EnumerateGroups2>
    {
        public EnumerateGroups2(MgSite site, string unitTestVm)
            : base(site, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "USER", param);
                _unitTestVm.ReadParameterValue(paramSetId, "LOGIN", param);
                _unitTestVm.ReadParameterValue(paramSetId, "PASSWORD", param);

                var userInfo = new MgUserInformation();
                userInfo.SetMgUsernamePassword(param["LOGIN"], param["PASSWORD"]);
                userInfo.SetLocale("en");

                var site = new MgSite();
                site.Open(userInfo);

                MgByteReader reader = site.EnumerateGroups(param["USER"]);
                site.Close();

                return TestResult.FromByteReader(reader);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class EnumerateRoles2 : SiteServiceOperationExecutor<EnumerateRoles2>
    {
        public EnumerateRoles2(MgSite site, string unitTestVm)
            : base(site, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "USER", param);
                _unitTestVm.ReadParameterValue(paramSetId, "LOGIN", param);
                _unitTestVm.ReadParameterValue(paramSetId, "PASSWORD", param);

                var userInfo = new MgUserInformation();
                userInfo.SetMgUsernamePassword(param["LOGIN"], param["PASSWORD"]);
                userInfo.SetLocale("en");

                var site = new MgSite();
                site.Open(userInfo);

                MgStringCollection roles = site.EnumerateRoles(param["USER"]);
                site.Close();

                return new TestResult(CommonUtility.MgStringCollectionToString(roles), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class AddGroup : SiteServiceOperationExecutor<AddGroup>
    {
        public AddGroup(MgSite site, string unitTestVm)
            : base(site, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "GROUP", param);
                _unitTestVm.ReadParameterValue(paramSetId, "DESCRIPTION", param);

                _site.AddGroup(param["GROUP"] ?? "", param["DESCRIPTION"] ?? "");
                return new TestResult();
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class UpdateGroup : SiteServiceOperationExecutor<UpdateGroup>
    {
        public UpdateGroup(MgSite site, string unitTestVm)
            : base(site, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "GROUP", param);
                _unitTestVm.ReadParameterValue(paramSetId, "NEWGROUP", param);
                _unitTestVm.ReadParameterValue(paramSetId, "NEWDESCRIPTION", param);

                _site.UpdateGroup(param["GROUP"] ?? "", param["NEWGROUP"] ?? "", param["NEWDESCRIPTION"] ?? "");
                return new TestResult();
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class DeleteGroups : SiteServiceOperationExecutor<DeleteGroups>
    {
        public DeleteGroups(MgSite site, string unitTestVm)
            : base(site, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "GROUPS", param);

                MgStringCollection groups = CommonUtility.StringToMgStringCollection(param["GROUPS"]);
                _site.DeleteGroups(groups);

                return new TestResult();
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class GrantRoleMembershipsToGroups : SiteServiceOperationExecutor<GrantRoleMembershipsToGroups>
    {
        public GrantRoleMembershipsToGroups(MgSite site, string unitTestVm)
            : base(site, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "ROLES", param);
                _unitTestVm.ReadParameterValue(paramSetId, "GROUPS", param);

                MgStringCollection roles = CommonUtility.StringToMgStringCollection(param["ROLES"]);
                MgStringCollection groups = CommonUtility.StringToMgStringCollection(param["GROUPS"]);

                _site.GrantRoleMembershipsToGroups(roles, groups);

                return new TestResult();
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class RevokeRoleMembershipsFromGroups : SiteServiceOperationExecutor<RevokeRoleMembershipsFromGroups>
    {
        public RevokeRoleMembershipsFromGroups(MgSite site, string unitTestVm)
            : base(site, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "ROLES", param);
                _unitTestVm.ReadParameterValue(paramSetId, "GROUPS", param);

                MgStringCollection roles = CommonUtility.StringToMgStringCollection(param["ROLES"]);
                MgStringCollection groups = CommonUtility.StringToMgStringCollection(param["GROUPS"]);

                _site.RevokeRoleMembershipsFromGroups(roles, groups);

                return new TestResult();
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class EnumerateRoles : SiteServiceOperationExecutor<EnumerateRoles>
    {
        public EnumerateRoles(MgSite site, string unitTestVm)
            : base(site, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "USER", param);
                _unitTestVm.ReadParameterValue(paramSetId, "GROUP", param);

                MgStringCollection roles = _site.EnumerateRoles(param["USER"], param["GROUP"]);

                return new TestResult(CommonUtility.MgStringCollectionToString(roles), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class EnumerateServers : SiteServiceOperationExecutor<EnumerateServers>
    {
        public EnumerateServers(MgSite site, string unitTestVm)
            : base(site, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                MgByteReader reader = _site.EnumerateServers();
                return TestResult.FromByteReader(reader);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class AddServer : SiteServiceOperationExecutor<AddServer>
    {
        public AddServer(MgSite site, string unitTestVm)
            : base(site, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "NAME", param);
                _unitTestVm.ReadParameterValue(paramSetId, "DESCRIPTION", param);
                _unitTestVm.ReadParameterValue(paramSetId, "ADDRESS", param);

                _site.AddServer(param["NAME"], param["DESCRIPTION"], param["ADDRESS"]);
                return new TestResult();
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class UpdateServer : SiteServiceOperationExecutor<UpdateServer>
    {
        public UpdateServer(MgSite site, string unitTestVm)
            : base(site, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "OLDNAME", param);
                _unitTestVm.ReadParameterValue(paramSetId, "NEWNAME", param);
                _unitTestVm.ReadParameterValue(paramSetId, "NEWDESCRIPTION", param);
                _unitTestVm.ReadParameterValue(paramSetId, "NEWADDRESS", param);

                _site.UpdateServer(param["OLDNAME"], param["NEWNAME"], param["NEWDESCRIPTION"], param["NEWADDRESS"]);
                return new TestResult();
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class RemoveServer : SiteServiceOperationExecutor<RemoveServer>
    {
        public RemoveServer(MgSite site, string unitTestVm)
            : base(site, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "NAME", param);

                _site.RemoveServer(param["NAME"]);
                return new TestResult();
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    /*
    public class EnumerateServicesOnServer : SiteServiceOperationExecutor<EnumerateServicesOnServer>
    {
        public EnumerateServicesOnServer(MgSite site, string unitTestVm)
            : base(site, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                return new TestResult();
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class AddServicesToServer : SiteServiceOperationExecutor<AddServicesToServer>
    {
        public AddServicesToServer(MgSite site, string unitTestVm)
            : base(site, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                return new TestResult();
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }
    
    public class RemoveServicesFromServer : SiteServiceOperationExecutor<RemoveServicesFromServer>
    {
        public RemoveServicesFromServer(MgSite site, string unitTestVm)
            : base(site, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                return new TestResult();
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }
     */
}
